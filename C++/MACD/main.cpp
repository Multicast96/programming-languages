#include<fstream>
#include<string>
#include<vector>
#include<stdexcept>
#include<math.h>
#include<utility>
#include<list>
#include<iomanip>
#include<iostream>

using namespace std;

void ReadDataFromFile(string fileName, vector<double> *data){
	string line;
	ifstream file(fileName, ios::out);
	if (!file.is_open()) {
		file.close();
		throw runtime_error("Couldn't open " + fileName);
	}
	while (getline(file, line)) data->push_back(stod(line));
	file.close();
}

void WriteToFile(string fileName,const vector<double> data) {
	ofstream file(fileName);
	if (!file.is_open()) {
		file.close();
		throw runtime_error("Couldn't open " + fileName);
	}
	for each (double var in data){
		file << var<<"\n";
	}
	file.close();
}

/* EMA - Exponential Moving Average*/
void CalculateEMA(int interval, const vector<double> data, vector<double> *result) {
	double alfa = 2 / (interval - 1);

	for (int dataIndex = 0; dataIndex < data.size(); dataIndex++) {
		double Numerator = 0; //licznik
		double Denominator = 0; //mianownik
		double tmp = 0; //Zmienna tymczasowa do obliczania (1-alfa)^i
		
		for (int i = 0; i <= interval; i++) {
			if (dataIndex - i >= 0) {
				tmp = pow(1 - alfa, i);
				Numerator += data[dataIndex - i]*tmp;
				Denominator += tmp;
			}
			else break;
		}

		result->push_back(Numerator / Denominator);
	}
}

/* Na podstawie wektora danych vector<double> data obliczane s¹ wektory MACD i Signal*/
void CalculateMACDandSignal(const vector<double> data, vector<double> *macd, vector<double> *signal) {
	vector<double> EMA_12; 
	vector<double> EMA_26; 
	CalculateEMA(12, data, &EMA_12);
	CalculateEMA(26, data, &EMA_26);

	for (int i = 0; i < data.size(); i++) {
		macd->push_back(EMA_12[i] - EMA_26[i]);
	}
	
	CalculateEMA(9, *macd, signal);
}

void StockPlayer(const vector<double> data, const vector<double> macd, const vector<double> signal) {
	int shares = 1000;
	double wallet = 0;
	double startValue = floor(shares*data[0]*100)/100.0;
	list<pair<int,double>> transactions;
	transactions.push_front(make_pair(1000, data[0]));
	std::cout << "Akcje: " <<setprecision(2)<< shares << " portfel: " << wallet << " zl\n";
	std::cout << "Wartosc akcji + portfel: " << fixed<< startValue<< " zl\n"; //!!! Zmieniæ to to_string
	std::cout << "Start symulacji\n";

	int sharesPerTransaction = 100; //procentowa ilosc akcji jakie mo¿emy sprzedac podczas jednej transakcji
	int cashSpendPerTransaction = 100; //procentowa ilosc pieniedzy z portfela, jakie mo¿emy wydaæ podczas jednej transakcji
	for (int i = 26; i < data.size(); i++) {
		if (macd[i] < signal[i] && macd[i-1] > signal[i]) {
				//mo¿liwoœæ sprzeda¿y
				for (auto it = transactions.begin(); it != transactions.end();) {
					if (it->second < data[i]) {
						//sprzedajemy
						int maxSharesToSell = shares * sharesPerTransaction/100; // maksymalna Iloœæ akcjii do sprzeda¿y podczas tej transakcji
						if (maxSharesToSell == 0) continue;
						if (it->first > maxSharesToSell) {
							shares -= maxSharesToSell;
							wallet += maxSharesToSell*data[i];
							it->first -= maxSharesToSell;
							it++;
						}
						else {
							shares -= it->first;
							wallet += it->first * data[i];
							it = transactions.erase(it);
						}
					}
					else it++;
				}
			}
		else if (macd[i] > signal[i] && macd[i-1] < signal[i-1]) {
				//kupujemy
				double maxCashToSpend = wallet * cashSpendPerTransaction/100;
				if (maxCashToSpend == 0) continue;
				int sharesBought = maxCashToSpend / data[i]; //iloœæ akcji jakie mo¿emy kupiæ wynosi max_pieniadze_do_wydania / cena_jednej_akcji
				if (sharesBought == 0) continue;
				shares += sharesBought;
				wallet -= sharesBought * data[i];
				transactions.push_back(make_pair(sharesBought, data[i]));
			}
	}
	std::cout << "--------------------------------\n";
	std::cout << "Koniec symulacji\n";
	std::cout << "Akcje: " << shares << " portfel: " << wallet << " zl\n";
	std::cout << "Wartosc akcji + portfel: " << shares*data.back() +wallet << " zl\n";
	std::cout << "Zysk: " << wallet + shares*data.back() - startValue<< " zl\n";
}

int main() {
	vector<double> data, macd, signal;

	try {
		ReadDataFromFile("data.txt", &data);
		CalculateMACDandSignal(data, &macd, &signal);
		WriteToFile("MACD.txt", macd);
		WriteToFile("Signal.txt", signal);
		StockPlayer(data, macd, signal);
	}
	catch (const exception &e) {
		std::cout << e.what() << endl;
	}
	return 0;
}