/* Plik dod.c — testowanie bibliotek statycznych w Linuksie */
int dodaj(int a, int b)
{
	int suma;
	suma = a + b;
	return suma;
}
int dodaj_kwadraty(int a, int b)
{
	int suma;
	suma = a*a + b*b;
	return suma;
}