#include <iostream>
#include <string>

using namespace std;

template<typename T>
class Base {
public:
    virtual ~Base() = default;
    virtual void Interface() const = 0;
};

class Derived: public Base<Derived> {
public:
    void Interface() const {
        cout << "Do Interface." << endl;
    }
};

int main ()
{
    Base<Derived> *b = new Derived();
    b->Interface();

    return 0;
}
