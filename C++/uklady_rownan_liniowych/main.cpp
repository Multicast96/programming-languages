#include"matrix.h"
#include<Windows.h>
#include<chrono>
// Kamil Dakus 165227
//ZAD A
//a1 = 5 + e | e = 2 | czwarta cyfra indeksu
//a2 = a3 = -1
//f = 5 | trzecia cyfra indeksu
const int N = 927;
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
	cout << "Liczba iteracji: " << iters << endl;
	cout << "Czas: " << elapsed.count() <<" ms"<< endl;
}

int main() {
	try {
		dataForTaskC();
		measureExecTime(gauss_seidl, A, x, b, 1e-9);
		dataForTaskC();
		measureExecTime(jacobi, A, x, b, 1e-9);
	}
	catch (exception &e) {
		cout << e.what() << endl;
	}
	system("pause");
}