int main(int ac, char **av)
{
	StringID wizard = StringID("Wizard");

	std::vector<StringID> inventory;
	inventory.push_back(StringID("Red rock"));
	inventory.push_back(StringID("Magic book"));
	inventory.push_back(StringID("Hat"));

	auto gnome = StringID("Gnome");

	if (wizard != gnome)
	{
		StringID magicBook;
		for (int i = 0; i < inventory.size(); ++i)
		{
			if (inventory[i] == StringID("Magic book"))
				break;
		}
		auto magicSpell = StringID("Abracadabra !");
		auto magicSpellStr = magicSpell.str();
		if (magicSpellStr == nullptr)
		{
			std::cout << "No debug spell availlable !" << std::endl;
			std::cout << "Gnome win !" << std::endl;
		}
		else
		{
			std::cout << magicSpell.str() << std::endl;
			std::cout << "Wizard win" << std::endl;

		}
	}
	return 0;
}