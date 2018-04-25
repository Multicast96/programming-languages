#include"matrix.h"

ostream& operator<<(ostream & os, const matrix & m)
{
	for (int i = 0; i < m.y; i++) {
		for (int j = 0; j < m.x; j++) {
			if (m.self[i][j] >= 0) os << " ";
			os << m.self[i][j] << " ";
		}
		os << "\n";
	}
	return os;
}

matrix::matrix(int x, int y, int value) : x(x) , y(y){
	self = new double*[y];

	for (int i = 0; i < y; i++) {
		self[i] = new double[x];
		for (int j = 0; j < x; j++) {
			self[i][j] = value;
		}
	}
}

double residuum_norm(matrix &A, matrix &x, matrix& b) {
	matrix res = A * x - b;
	if(res.x != 1) throw runtime_error("Error 2: Residuum must be a vector.");
	double norm = 0;
	for (int i = 0; i < res.y; i++) {
		norm += res[i][0] * res[i][0];
	}
	return sqrt(norm);
}

int faktoryzacja_LU(matrix &A, matrix& x, matrix& b, double precision) {
	cout << "Faktoryzacja LU  ";
	const int N = A.y;
	matrix L(N, N);
	matrix U(N, N);
	for (int i = 0; i < N; i++) L[i][i] = 1;

	//Podzia³ na L i U
	for (int i = 0; i < N; i++) {
		for (int j = i; j < N; j++) {
			U[i][j] += A[i][j];
			for (int k = 0; k <= i - 1; k++) {
				U[i][j] -= L[i][k] * U[k][j];
			}
		}

		for (int j = i + 1; j < N; j++) {
			for (int k = 0; k <= i - 1; k++) {
				L[j][i] -= L[j][k] * U[k][i];
			}
			L[j][i] += A[j][i];
			L[j][i] /= U[i][i];
		}
	}

	//Ly=b
	for (int i = 0; i < N; i++) {
		x[i][0] = b[i][0];
		for (int j = 0; j < i; j++) {
			x[i][0] -= L[i][j] * x[j][0];
		}
		for (int j = i + 1; j < N; j++) {
			x[i][0] -= L[i][j] * x[j][0];
		}
		x[i][0] /= L[i][i];
	}

	//Ux=y
	for (int i = N-1; i >= 0; i--){
		x[i][0] = x[i][0];
		for (int j = N-1; j > i; j--) {
			x[i][0] -= U[i][j] * x[j][0];
		}
		for (int j = i - 1; j >= 0; j--) {
			x[i][0] -= U[i][j] * x[j][0];
		}
		x[i][0] /= U[i][i];
	}

	return 0;
}

int gauss_seidl(matrix & A, matrix & x, matrix & b, double precision){
	cout << "Gauss seidl      ";
	int iter = 0;
	double res = 1;
	while (res >= precision) {
		for (int i = 0; i < x.y; i++) {
			double sum1 = 0, sum2 = 0;
			for (int j = 0; j <= i - 1; j++) {
				sum1 += A[i][j] * x[j][0];
			}
			for (int j = i+1; j < x.y; j++) {
				sum2 += A[i][j] * x[j][0];
			}
			x[i][0] = (b[i][0] - sum1 - sum2) / A[i][i];
		}
		res = residuum_norm(A, x, b);
		++iter;
	}
	return iter;
}

int jacobi(matrix & A, matrix & x, matrix & b, double precision) {
	cout << "Jacobi           ";
	int iter = 0;
	double res = 1;
	while (res >= precision) {
	matrix x1(1, x.y);
		for (int i = 0; i < x.y; i++) {
			double sum1 = 0, sum2 = 0;
			for (int j = 0; j <= i - 1; j++) {
				sum1 += A[i][j] * x[j][0];
			}
			for (int j = i + 1; j < x.y; j++) {
				sum2 += A[i][j] * x[j][0];
			}
			x1[i][0] = (b[i][0] - sum1 - sum2) / A[i][i];
		}
		x = (move(x1));
		res = residuum_norm(A, x, b);
		++iter;
	}
	return iter;
}

matrix::matrix(matrix&& source) : x(source.x), y(source.y) {
	self = source.self;
	source.self = nullptr;
}

matrix::matrix(){}

matrix::~matrix() {
	if (self == nullptr) return;
	for (int i = 0; i < y; i++) {
		delete[] self[i];
	}
	delete[] self;
}

void matrix::operator=(matrix&& source){
	x = source.x;
	y = source.y;
	self = source.self;
	source.self = nullptr;
}

matrix matrix::operator*(matrix & m){
	if (x != m.y) throw runtime_error("Error 1: Inner matrix dimensions must agree.");
	matrix result(m.x, y);
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < m.x; j++) {
			for (int k = 0; k < m.y; k++) {
				result[i][j] += self[i][k] * m[k][j];
			}
		}
	}
	return matrix(move(result));
}

matrix matrix::operator-(matrix & m) {
	if (x != m.x || y != m.y) throw runtime_error("Error 2: Matrix dimensions must agree.");
	matrix result(x, y);
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			result[i][j] = self[i][j] - m[i][j];
		}
	}
	return matrix(move(result));
}

double * matrix::operator[](int i){
	return self[i];
}

void matrix::init1(int a1, int a2) {
	int start = 0;
	int jStart = 0;
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++) {
			jStart = start + j;
			if (jStart == -2 || jStart == -1 || jStart == 1 || jStart == 2) self[i][j] = a2;
			else if (jStart == 0) self[i][j] = a1;
			else self[i][j] = 0;
		}
		start--;
	}
}

void matrix::init2(int a1, int a2) {}

void matrix::init3(int f) {
	for (int i = 0; i < y; i++) {
		self[i][0] = sin(i *(f + 1));
	}
}

