/* Plik test_bibl.c — testowanie bibliotek statycznych i dynamicznych w Linuksie */
#include <stdio.h>
int main()
{
	int suma, sumakw;
	suma = dodaj(5, 12);
	sumakw = dodaj_kwadraty(3, 5);
	printf("\nSuma = %d Suma kwadratow = %d\n", suma, sumakw);
	return 0;
}