#pragma once
#include "Class.h"
class SubClass2 :
    public Class
{
public:
    SubClass2();
    int GetN2() { return num2; }
private:
    static int num2;
};

