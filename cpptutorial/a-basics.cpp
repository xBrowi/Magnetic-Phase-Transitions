// hvis man er i tvivl om noget som helst nogensinde: spørg Google, https://www.learncpp.com/, eller dokumentationen https://en.cppreference.com/ (fra oldtiden)

//inkludér biblioteker:
#include <iostream> // cout (print), cin (input)
#include <string> // string datatype
#include <vector> // vector datatype (array med dynamisk størrelse)

/*
c++ er whitespace insensitive i stedet for ":" + indentation som i Python, bruges {} til at definere kodeblokke.
(næsten) alle linjer afsluttes med semikolon ";" i stedet for linjeskift som i Python.
*/

// Main funktionen. Bliver kørt når programmet starter.
int main() {

    // std:: er et "namespace" svarende til "np." i Python. Cout printer (print() i Python). ( << er "insertion operator", ik tænk over det)
    std::cout << "Hello, world!\n";


    std::cout << "\nVariable:\n";

    // variabler skal defineres med en type:
    int myNumber = 5; //heltal
    float myFloat = 5.99; //decimaltal
    std::string myText = "Hello"; //tekststreng
    bool myBool = true; //sand/falsk værdi
    int array[5] = {2, 4, 6, 8, 10}; //array af heltal med fast størrelse 5 (men lad være med at bruge dem, brug std::vector i stedet)

    // man kan også definere variabler uden at initialisere dem:
    int anotherNumber; // deklarer variablen, har den værdi som tilfældigvis ligger i hukommelsen
    std::cout << "anotherNumber: " << anotherNumber << "\n"; //udskriver værdien (ukendt)

    anotherNumber = 10; //tildel værdi senere
    std::cout << "anotherNumber: " << anotherNumber << "\n"; //udskriver 10

    // hvis man skal bruge højere præcision findes og "double" og "long double" typerne. 



    std::cout << "\nFor loops:\n";
    // For loops defineres meget eksplicit med initialization, condition og increment/decrement. kører så længe condition er sand (kan være uendeligt), så pas på

    for (int i = 0; i < 5; i++) {
        std::cout << i << "\n";
    }

    std::cout << "\n";

    // man kan også lave "for i in array" på denne måde:
    for (int i : array) {
        std::cout << i << "\n"; //udskriver hvert element i arrayet
    }



    std::cout << "\nVectors:\n";
    
    // vector er et array med dynamisk størrelse. Findes også i std namespace. Eksempel på en vector af heltal:
    std::vector<int> myVector = {1, 2, 3, 4, 5}; //initialiseret med 5 elementer

    std::cout << myVector[0] << "\n"; //udskriver første element (1)

    // der er forskellige "methods" (funktioner tilknyttet en datatype) til vector. En af dem er size(), som returnerer antallet af elementer i vectoren:
    std::cout << "Vector size: " << myVector.size() << "\n";

    // en anden er push_back(), som tilføjer et element til slutningen af vectoren:
    myVector.push_back(6); //tilføjer 6 til slutningen af vector

    std::cout << "New vector size: " << myVector.size() << "\n"; //udskriver den nye størrelse (6)




    return 0; //Returnerer 0 for at indikere at programmet er færdigt uden fejl. (ligegyldigt i små programmer, teknisk set unødvendigt)
}


//for at kompilere køre programmet, tryk på pilen eller ctrl+alt+n i VSCode med Code Runner extension installeret.
// HUSK AT GEMME INDEN!!!!
