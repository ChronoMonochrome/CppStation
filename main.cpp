#include <iostream>

#include <memory/bios.hpp>
#include "helpers.hpp"

int main()
{
	auto res = bios::getBios();
	{
		std::shared_ptr<bios::Bios> mybios(res.check());
		cout << mybios->mIsValidImage << endl;
	}

	system("pause");
	return 0;
}