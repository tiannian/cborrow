#include <iostream>

using namespace std;

#include <borrow.h>

class test: public cborrow::_internal::_linked_header {
public:
    uint32_t v;
    test(uint32_t v): v(v) {}
    test(const test &t, uint32_t v):_linked_header(t), v(v) {}
};

int main() {
    test t0(0);
    test t1(t0,1);
    test t2(t0,2);
    test t3(t0,3);
    test t4(t0,4);
    test t5(t0,5);
    test t6(t0,6);
    {
        test t7(t0,7);
        test t8(t0,8);
        t0.map([](cborrow::_internal::_linked_header *h){
            test *p = (test*)(h);
            cout << p->v << endl;
        });
    }
    t0.map([](cborrow::_internal::_linked_header *h){
        test *p = (test*)(h);
        cout << p->v << endl;
    });
    return 0;
}
