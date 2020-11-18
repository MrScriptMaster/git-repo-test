#include <iostream>

class SomeClass {
    public:
        class Builder {
            friend class SomeClass;
            private:
                int value1;
                int value2;
                int value3;
                int value4;
                int value5;
            
            public:
                Builder(int value1, int value2) : 
                  value1(0)
                , value2(0) 
                , value3(0) 
                , value4(0) 
                , value5(0) 
                {
                    this->value1 = value1;
                    this->value2 = value2;
                }
                Builder& setValue1(int v) {
                    value1 = v;
                    return *this;    
                }
                Builder& setValue2(int v) {
                    value2 = v;
                    return *this;    
                }
                Builder& setValue3(int v) {
                    value3 = v;
                    return *this;    
                }
                Builder& setValue4(int v) {
                    value4 = v;
                    return *this;    
                }
                Builder& setValue5(int v) {
                    value5 = v;
                    return *this;    
                }
                SomeClass& build() {
                    return *(new SomeClass(*this));
                }
        };
    private:
        int value1; // обязательный
        int value2; // обязательный
        int value3; // не обязательный
        int value4; // не обязательный
        int value5; // не обязательный

    protected:
        friend class Builder;
        SomeClass(const Builder& b) {
            value1 = b.value1;
            value2 = b.value2;
            value3 = b.value3;
            value4 = b.value4;
            value5 = b.value5;
        }

    public:
        virtual ~SomeClass() {
            std::cout << "dest" << std::endl;
        }
        
        void print() {
            std::cout << value1 << " " << value2 << " " << value3 << " " << value4 << " " << value5 << " " << std::endl;
        }

        
};

int main(int argc, char* argv[]) {
    SomeClass inst = SomeClass::Builder(1, 2).setValue5(19).setValue3(18).build();
    inst.print();
    return 0;
}
