/* Plik odej.c — testowanie bibliotek statycznych w Linuksie */
int odejmij(int a, int b)
{
	int roznica;
	roznica = a - b;
	return roznica;
}
int odejmij_kwadraty(int a, int b)
{
	int wartosc;
	wartosc = a*a - b*b;
	return wartosc;
}