#include"matrix.h"
#include<Windows.h>
#include<chrono>
// Kamil Dakus 165227
//ZAD A
//a1 = 5 + e | e = 2 | czwarta cyfra indeksu
//a2 = a3 = -1
//f = 5 | trzecia cyfra indeksu
int N = 20;
matrix A,b,x;

void init() {
	A = matrix(N, N);
	b = matrix(1, N);
	x = matrix(1, N, 1); // wektor wyniku na pocz¹tku wszytkie wartoœci 1 
}

void dataForTaskA() {
	init();
	A.init1(7, -1);
	b.init3(5);
}

void dataForTaskC() {
	init();
	A.init1(3, -1);
	b.init3(5);
}

void measureExecTime(int (*fun)(matrix&, matrix&, matrix&, double), matrix& A, matrix&x, matrix&b, double precision) {
	auto start = chrono::system_clock::now();
	int iters = fun(A,x,b,precision);
	auto end = chrono::system_clock::now();
	auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
	cout << "N = "<<A.y<<" Liczba iteracji: " << iters<<" Czas: " << elapsed.count() <<" ms"<< endl;
}

int main() {
	try {
		cout << "Zad A" << endl;
		N = 15;
		dataForTaskA();
		cout << A << endl << endl;;
		cout << "Zad B" << endl;
		N = 927;
		dataForTaskA();
		measureExecTime(gauss_seidl, A, x, b, 1e-9);
		dataForTaskA();
		measureExecTime(jacobi, A, x, b, 1e-9);

		cout << endl << "Zad C" << endl;
		dataForTaskC();
		measureExecTime(gauss_seidl, A, x, b, 1e-9);
		dataForTaskC();
		measureExecTime(jacobi, A, x, b, 1e-9);

		cout << endl << "Zad D" << endl;
		dataForTaskC();
		measureExecTime(faktoryzacja_LU, A, x, b, 0);
		cout << "Norma z residuum wynosi 0" << endl;

		cout << endl << "Zad E" << endl;

		const int values[] = { 100,500,1000,2000,3000 };

		for (int i = 0; i < sizeof(values) / sizeof(int); i++) {
			N = values[i];
			dataForTaskA();
			measureExecTime(jacobi, A, x, b, 1e-9);
		}

		for (int i = 0; i < sizeof(values) / sizeof(int); i++) {
			N = values[i];
			dataForTaskA();
			measureExecTime(gauss_seidl, A, x, b, 1e-9);
		}

		for (int i = 0; i < sizeof(values) / sizeof(int); i++) {
			N = values[i];
			dataForTaskA();
			measureExecTime(faktoryzacja_LU, A, x, b, 0);
		}
	}
	catch (exception &e) {
		cout << e.what() << endl;
	}
	system("pause");
}