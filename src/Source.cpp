#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <algorithm>
using namespace std;

const string rodzaj_bramki[] = { "NAND", "AND", "NOR", "XOR", "XNOR", "OR", "NEG" };
unsigned int ilosc_przypadkow = 0;
struct stan {
	unsigned int linia;
	bool wartosc;
	stan *nast = nullptr;
};
struct wejscie {
	unsigned int nr;
	stan* glowa_stan = nullptr;
	stan* ogon_stan = nullptr;
	wejscie *nast=nullptr;
};
struct wyjscie {
	unsigned int nr;
	wyjscie *nast= nullptr;
};
struct bramka {
	short typ;
	unsigned int  nr_wejscia1, nr_wejscia2, nr_wyjscia;
	bramka *nast=nullptr;
};

wejscie* glowa_wejscie = nullptr;
wyjscie* glowa_wyjscie = nullptr;
bramka* glowa_bramka = nullptr;

wejscie* ogon_wejscie = nullptr;
wyjscie* ogon_wyjscie = nullptr;
bramka* ogon_bramka = nullptr;

void usun_struktury();
void pomoc();
void walidacja(string &ln);
int pobierz_liczbe(string &ln);
int pobierz_bramke(string &ln);
bool wczytaj_in(string ln);
bool wczytaj_out(string ln);
bool wczytaj_bramke(string ln);
wejscie* szukaj_wejscia(int nr_wejscia);
void dodaj_stan(wejscie* w_wejscia, bool stan_ln, int nr_ln);
bool wczytaj_stany(string ln, int nr_ln);
bool wyjscie_bramka(int rodzaj, bool w1, bool w2);
bramka* szukaj_bramki(int nr_wyjscia);
stan* szukaj_stan(stan* glowa_stan, int nr_ln);
bool wynik_wezla(int nr_wezla, int nr_ln);
bool przelaczniki(int argc, char* argv[], string &src_uklad, string &src_wejscia, string &src_wyjscia);
void usun_struktury();
void wyswietl_struktury(void);//pomocnicza funkcja na etapie pisania
bool zapisz_wynik(string src_wyjscia, string wynik);
bool wczytaj_uklad(string src_uklad);
bool wczytaj_stan_wejsc(string src_wejscia);
void analizuj(string &wynikowy);


int main(int argc, char* argv[]) {
	string src_uklad = "", src_wejscia = "", src_wyjscia = "";
	if (!przelaczniki(argc, argv, src_uklad, src_wejscia, src_wyjscia)) {
		cout << "Nieprawidlowe podane parametry uruchomienia!" << endl;
		pomoc();
		usun_struktury();
		return 0;
	}
	if (!wczytaj_uklad(src_uklad)) {
		usun_struktury();
		return 0;
	}
	if (!wczytaj_stan_wejsc(src_wejscia)) {
		usun_struktury();
		return 0;
	}
		
	// - - - oblicz i zapisz wyniki start
	string wynikowy;
	analizuj(wynikowy);
	zapisz_wynik(src_wyjscia, wynikowy);

	// - - - oblicz i zapisz wyniki koniec
	usun_struktury();
	cout << "Analiza pliku: [" << src_uklad << "] na podstawie danych z pliku: [" << src_wejscia << "] zostala zapisana do pliku: [" << src_wyjscia << "].\nOperacja zakonczona pomyslnie!" << endl;
	return 0;
}

void pomoc() {
	cout<< "========================================================================================================================" << endl
		<< "======================================================== Pomoc: ========================================================" << endl
		<< "\tAnalizuje uklady bramek logicznych."
		<< "\tTUC.exe -u <sciezka> -i <sciezka> -o <sciezka>" << endl
		<< "\t-u \t ustawia sciezka do pliku z analizowanym ukladem" << endl
		<< "\t-i \t ustawia sciezka do pliku z stanami wejsciowymi" << endl
		<< "\t-o \t ustawia sciezka do pliku wyjsciowego (w ktorym maja zostac zapisane wyniki)" << endl
		<< "\t-h \t wyswietla pomoc" << endl
		<< "========================================================================================================================" << endl;
}

void walidacja(string &ln) {
	for (int i = 0; i < ln.length(); i++) {
		ln[i] = toupper(ln[i]);
		if (ln[i] == ';')
			ln[i] = ':';
		if (ln[i] == '\t')
			ln[i] = ' ';
		if (ln[i] == ' ' && ln[i - 1] == ' ' && i > 0)
			ln.erase(i, 1);
		if (ln[i] == ':' && ln[i - 1] == ' ' && i > 0)
			ln.erase((i - 1), 1);
	}
	int i = 0;
	do {
		if (ln[i] == ' ')
			ln.erase(i, 1);
		else
			i++;
	} while ((ln[i] < 48 || ln[i]>57) && (i < ln.length()));
	ln += ' ';
}

int pobierz_liczbe(string &ln) {
	string liczba="";
	for (int i = 0; i < ln.length(); i++) {
		if (48 <= ln[i]&&ln[i] <= 57)
			liczba += ln[i];
		else
			if (liczba != "") {
				ln.erase(0,i);
				return stoi(liczba);
			}
	}
	return -1;
}

int pobierz_bramke(string &ln) {
	for(int i=0;i<7;i++)
	if (ln.find(rodzaj_bramki[i]) != string::npos) {
		ln.erase(0, ln.find(rodzaj_bramki[i]) + rodzaj_bramki[i].length());
		return i;
	}
	return -1;

}

bool wczytaj_in(string ln) {
	walidacja(ln);
	if (ln.find("IN:") != string::npos) {
		ln = ln.substr(ln.find("IN:") + 3);
		int liczba = pobierz_liczbe(ln);;
		while (liczba != -1) {
			if(glowa_wejscie == nullptr){
				glowa_wejscie = new wejscie;
				glowa_wejscie->nr = liczba;
				ogon_wejscie = glowa_wejscie;
			}
			else {
				ogon_wejscie->nast = new wejscie;
				ogon_wejscie = ogon_wejscie->nast;
				ogon_wejscie->nr = liczba;
			}

			liczba = pobierz_liczbe(ln);
		}
		if (glowa_wejscie == nullptr)
			return 0;
	}
	else 
		return 0;
	return 1;
}

bool wczytaj_out(string ln) {
	walidacja(ln);
	if (ln.find("OUT:") != string::npos) {
		ln = ln.substr(ln.find("OUT:") + 4);
		int liczba = pobierz_liczbe(ln);
		while (liczba != -1) {
			if (glowa_wyjscie == nullptr) {
				glowa_wyjscie = new wyjscie;
				glowa_wyjscie->nr = liczba;
				ogon_wyjscie = glowa_wyjscie;
			}
			else {
				ogon_wyjscie->nast = new wyjscie;
				ogon_wyjscie = ogon_wyjscie->nast;
				ogon_wyjscie->nr = liczba;
			}
			liczba = pobierz_liczbe(ln);
		}
		if (glowa_wyjscie == nullptr)
			return 0;
	}
	else
		return 0;
	return 1;
}

bool wczytaj_bramke(string ln) {
	walidacja(ln);
	
		ln = ln + " ";
		int liczba=-1, typ = pobierz_bramke(ln);
		if (typ < 0)
			return 0;
		if (glowa_bramka == nullptr) {
			glowa_bramka = new bramka;
			ogon_bramka = glowa_bramka;
		}
		else {
			ogon_bramka->nast = new bramka;
			ogon_bramka = ogon_bramka->nast;
		}
		ogon_bramka->typ = typ;
				

		liczba = pobierz_liczbe(ln);
		if (liczba < 0)
			return 0;
				ogon_bramka->nr_wejscia1 = liczba;
		if (typ != 6) {
			liczba = pobierz_liczbe(ln);
			if (liczba < 0)
				return 0;
			ogon_bramka->nr_wejscia2 = liczba;
		}
		else
			ogon_bramka->nr_wejscia2 = liczba;
			liczba = pobierz_liczbe(ln);
			if (liczba < 0)
				return 0;
			if (szukaj_bramki(liczba) != nullptr) {
				cout << "Bramka o podanym numerze wyjscia juz istnieje! ";
				return 0;
			}
			ogon_bramka->nr_wyjscia = liczba;

	return 1;
}

wejscie* szukaj_wejscia(int nr_wejscia) {
	wejscie* w_wejscie = glowa_wejscie;
	while (w_wejscie != nullptr ) {
		if (w_wejscie->nr == nr_wejscia)
			return w_wejscie;
		w_wejscie = w_wejscie->nast;	
	}
	return w_wejscie;
}

void dodaj_stan(wejscie* w_wejscia, bool stan_ln, int nr_ln) {
	if (w_wejscia->glowa_stan == nullptr) {
		w_wejscia->glowa_stan = new stan;
		w_wejscia->ogon_stan = w_wejscia->glowa_stan;
	}
	else {
		w_wejscia->ogon_stan->nast = new stan;
		w_wejscia->ogon_stan = w_wejscia->ogon_stan->nast;
	}
	w_wejscia->ogon_stan->wartosc = stan_ln;
	w_wejscia->ogon_stan->linia = nr_ln;

}

bool wczytaj_stany(string ln, int nr_ln) {
	walidacja(ln);
	int nr=-1, stan=-1;
	nr = pobierz_liczbe(ln);
	stan = pobierz_liczbe(ln);
	while (nr >= 0 && stan >= 0) {
		if (nr<0 && stan != 0 && stan != 1)
			return 0;
		wejscie* w_wejscie = szukaj_wejscia(nr);
		if (w_wejscie == nullptr)
			return 0;
		dodaj_stan(w_wejscie, stan, nr_ln);
		nr = pobierz_liczbe(ln);
		stan = pobierz_liczbe(ln);

	} 
	return 1;

}

bool wyjscie_bramka(int rodzaj, bool w1, bool w2) {
	switch (rodzaj) {
	case 0:
		return !(w1 && w2);//NAND
	case 1:
		return (w1 && w2);//AND
	case 2:
		return !(w1 || w2);//NOR
	case 3:
		return (!w1 == w2);//XOR
	case 4:
		return (w1 == w2);//XNOR
	case 5:
		return (w1 || w2);//OR
	case 6:
		return !w1;//NOT
	default:
		cout << "Niezidentyfikowana bramka!" << endl;
		usun_struktury();
		exit(0);
	}
}

bramka* szukaj_bramki(int nr_wyjscia) {
	bramka* w_bramka = glowa_bramka;
	while (w_bramka != nullptr) {
		if (w_bramka->nr_wyjscia == nr_wyjscia)
			return w_bramka;
		w_bramka = w_bramka->nast;
	}
	return w_bramka;
}

stan* szukaj_stan(stan* glowa_stan, int nr_ln) {
	stan* w_stan = glowa_stan;
	while (w_stan != nullptr) {
		if (w_stan->linia == nr_ln)
			return w_stan;
		w_stan = w_stan->nast;
	}
	return w_stan;
}

bool wynik_wezla(int nr_wezla, int nr_ln) {
	bramka* w_bramka = szukaj_bramki(nr_wezla);
	//cout << "szukam" << nr_wezla << endl;
	if (w_bramka == nullptr) {
		wejscie* w_wejscie = szukaj_wejscia(nr_wezla);
		if(w_wejscie!=nullptr){
			stan* w_stan = szukaj_stan(szukaj_wejscia(nr_wezla)->glowa_stan, nr_ln);
			if(w_stan!=nullptr)
				return w_stan->wartosc;
			else {
				cout << "Blad stanu wezla nr:" << nr_wezla << endl;
				usun_struktury();
				exit(0);
			}
		}
		else {
			cout << "Nie znaleziono wezla nr:" << nr_wezla << endl;
			usun_struktury();
			exit(0);
		}
	}
	else {
		return wyjscie_bramka(w_bramka->typ, (wynik_wezla(w_bramka->nr_wejscia1, nr_ln)), wynik_wezla(w_bramka->nr_wejscia2, nr_ln));
	}
}

bool przelaczniki(int argc, char* argv[], string &src_uklad, string &src_wejscia, string &src_wyjscia) {
	string przelacznik = "";
	if (argc == 7) {
		for (int i = 1; i < argc; i += 2) {
			przelacznik = argv[i];
			//sciezka = argv[i + 1];
			if (przelacznik == "-u")
				src_uklad = argv[i + 1];

			else if (przelacznik == "-i")
				src_wejscia = argv[i + 1];

			else if (przelacznik == "-o")
				src_wyjscia = argv[i + 1];

			else if (przelacznik == "-h") {
				pomoc();
				exit(0);
			}
			else {
				cout << przelacznik << endl;
				return 0;
			}
		}

		//cout << "-u: " << src_uklad << endl << "-i: " << src_wejscia << endl << "-o: " << src_wyjscia << endl;
		return 1;
	}
	else if (argc == 2) {
		przelacznik = argv[1];
		if (przelacznik == "-h") {
			pomoc();
			exit(0);
		}
		else
			return 0;
	}
	else {
		return 0;
	}
}

void usun_struktury() {
	bramka* w_bramka = glowa_bramka;
	while (w_bramka != nullptr) {
		w_bramka = w_bramka->nast;
		delete glowa_bramka;
		glowa_bramka = w_bramka;
	}
	
	wyjscie* w_wyjscie = glowa_wyjscie;
	while (w_wyjscie != nullptr) {
		w_wyjscie = w_wyjscie->nast;
		delete glowa_wyjscie;
		glowa_wyjscie = w_wyjscie;
	}

	wejscie* w_wejscie = glowa_wejscie;
	while (w_wejscie != nullptr) {
		stan* w_stan = glowa_wejscie->glowa_stan;
		while (w_stan != nullptr) {
			w_stan = w_stan->nast;
			delete glowa_wejscie->glowa_stan;
			glowa_wejscie->glowa_stan=w_stan;
		}
		w_wejscie = w_wejscie->nast;
		delete glowa_wejscie;
		glowa_wejscie = w_wejscie;
	}
}

void wyswietl_struktury(void) {
		wejscie* w_wejscie = glowa_wejscie;
		do {
			cout << "Wejscie: || " << w_wejscie->nr << " || " << endl;
			w_wejscie = w_wejscie->nast;
		} while (w_wejscie != nullptr);

		wyjscie* w_wyjscie = glowa_wyjscie;
		do {
			cout << "Wyjscie: || " << w_wyjscie->nr << " || " << endl;
			w_wyjscie = w_wyjscie->nast;
		} while (w_wyjscie != nullptr);

		bramka* w_bramka = glowa_bramka;
		do {
			cout << "Bramka: " << w_bramka->typ << "|| " << w_bramka->nr_wejscia1 << ", " << w_bramka->nr_wejscia2 << ", " << w_bramka->nr_wyjscia << " || " << endl;
			w_bramka = w_bramka->nast;
		} while (w_bramka != nullptr); 
}

bool zapisz_wynik(string src_wyjscia, string wynik){
	fstream wyjsciowy;
	wyjsciowy.open(src_wyjscia, ios::out | ios::trunc);
	if (wyjsciowy.good()) {
		wyjsciowy << wynik;
		wyjsciowy << "\n \n \n TUC - Autor: S³awomir Krzyka³a";
		wyjsciowy.close();
		return 1;
	}
	else
		cout << "Blad otwarcia pliku: " << src_wyjscia << endl;
		return 0;
}

bool wczytaj_uklad(string src_uklad) {

	// - - - Wczytaj uk³ad start
	fstream uklad;
	uklad.open(src_uklad, ios::in);
	if (uklad.good()) {
		string linia;
		int nr_lini_uklad = 0;
		//start wczytywanie we/wy do struktur 
		do {
			nr_lini_uklad++;
			if (getline(uklad, linia) && linia.length() > 3) {
				if (!wczytaj_in(linia)) {
					cout << "Blad wczytywania wejsc ukladu - linia [" << src_uklad << " : "<< nr_lini_uklad <<"]!" << endl;
					return 0;
				}
			}
			else if (linia == "") {
				cout << "Blad w lini wejsc ukladu - linia [" << src_uklad << " : " << nr_lini_uklad << "] - pusta linia." << endl;
			}
			else {
				cout << "Blad w lini wejsc ukladu - linia [" << src_uklad << " : " << nr_lini_uklad << "]!" << endl;
				return 0;
			}
		} while (linia == "" && !uklad.eof());

		do {
			nr_lini_uklad++;
			if (getline(uklad, linia) && linia.length() > 4) {
				if (!wczytaj_out(linia)) {
					cout << "Blad wczytywania wyjsc ukladu - linia [" << src_uklad << " : " << nr_lini_uklad << "]!" << endl;
					return 0;
				}
			}
			else if (linia == "") {
				cout << "Blad w lini wyjsc ukladu - linia [" << src_uklad << " : " << nr_lini_uklad << "] - pusta linia." << endl;
			}
			else {
				cout << "Blad w lini wyjsc ukladu - linia [" << src_uklad << " : " << nr_lini_uklad << "]!" << endl;
				return 0;
			}
		} while (linia == "" && !uklad.eof());
		//konie wczytywanie we/wy do struktur 

		//start wczytywanie bramek do struktur 
		while (!uklad.eof()) {
			nr_lini_uklad++;
			if (getline(uklad, linia) && linia.length() > 5) {
				if (!wczytaj_bramke(linia)) {
					cout << "Blad wczytywania bramki ukladu - linia[" << src_uklad << " : " << nr_lini_uklad << "]!" << endl;
					return 0;
				}
			}
			else if (linia == "") {
				cout << "Blad w lini [" << src_uklad << " : " << nr_lini_uklad << "] bramki ukladu - pusta linia." << endl;
			}
			else {
				cout << "Blad w lini [" << src_uklad << " : " << nr_lini_uklad << "] bramki ukladu!" << endl;
				return 0;
			}
		}
		uklad.close();
	}
	else {
		cout << "Blad otwierania pliku ukladu: [" << src_uklad << "]!" << endl;
		return 0;
	}
	return 1;
	// - - - wczytaj uklad koniec
}

bool wczytaj_stan_wejsc(string src_wejscia) {
	// - - - wczytaj stan wejscia start
	fstream wejscia;
	string linia;
	wejscia.open(src_wejscia, ios::in);
	if (wejscia.good()) {
		int nr_lini_wejscia = 1;
		while (!wejscia.eof()) {
			if (getline(wejscia, linia) && linia.length() > 2) {
				if (!wczytaj_stany(linia, nr_lini_wejscia)) {
					cout << "Blad wczytywania wejsc - linia [" << src_wejscia << " : " << nr_lini_wejscia << "]" << endl;
					return 0;
				}
				else
					ilosc_przypadkow++;
			}
			else {
				cout << "Blad pobierania lini [" << src_wejscia << " : " << nr_lini_wejscia << "] wejscia" << endl;
				return 0;
			}
			nr_lini_wejscia++;
		}
		if (nr_lini_wejscia == 1) {
			cout << "Nie znaleziono lini wejsc - prawdopodobnie pusty plik [" << src_wejscia << "]!" << endl;
			return 0;
		}
	}
	else {
		cout << "Blad otwierania pliku wejscia: [" << src_wejscia << "]!" << endl;
		return 0;
	}
	return 1;
	// - - - wczytaj stan wejscia koniec
}

void analizuj(string &wynikowy) {
	for (int i = 1; i <= ilosc_przypadkow; i++) {
		wejscie* w_wejscie = glowa_wejscie;
		wynikowy += "IN: ";
		while (w_wejscie != nullptr) {
			wynikowy += to_string(w_wejscie->nr) + ":" + to_string(szukaj_stan(szukaj_wejscia(w_wejscie->nr)->glowa_stan, i)->wartosc) + " ";
			w_wejscie = w_wejscie->nast;
		}
		wyjscie* w_wyjscie = glowa_wyjscie;
		wynikowy += "\tOUT: ";
		while (w_wyjscie != nullptr) {
			wynikowy += to_string(w_wyjscie->nr) + ":" + to_string(wynik_wezla(w_wyjscie->nr, i)) + " ";
			w_wyjscie = w_wyjscie->nast;
		}
		wynikowy += "\n";
	}
}