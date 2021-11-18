#include <iostream>
#include <vector>
#include "Class.h"
#include "SubClass.h"
#include "SubClass2.h"

int main(int argc, char** argv)
{
	std::vector<Class*> CLASSES;

	SubClass* newSub = new SubClass;
	
	std::vector<SubClass2*> newSub2;
	for (int i = 0; i < 10; i++)
	{
		new SubClass2;
	}

	auto iter = CLASSES.begin();
	for (; iter != CLASSES.end(); ++iter)
	{
		printf("class : %d", (*iter)->GetN());
	}

	auto iter2 = newSub2.begin();
	for (; iter2 != newSub2.end(); ++iter2)
	{
		printf("subclass2 : %d", (*iter2)->GetN2());
	}

}