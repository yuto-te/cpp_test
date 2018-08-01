#include <iostream>
#include <string>

int main()
{
    using std::cin;
    using std::cout;
    using std::endl;
    using std::string;

    string name;
    int year;

    // name
    cout << "What your name?" << endl;
    cin >> name;
    // year
    cout << "How old are you?" << endl;
    cin >> year;

    cout << name << " san is " << year << " sai." << endl;
}