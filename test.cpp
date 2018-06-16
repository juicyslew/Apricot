#include <list>
#include <iostream>


struct wow{
	std::list<int> test;

	wow(){

	}
};

int main(){
	struct wow *wowie = new wow();
	std::cout << "test size: " << wowie->test.size() << std::endl;
	std::cout << "test empty: " << wowie->test.empty() << std::endl;
	return 0;
}