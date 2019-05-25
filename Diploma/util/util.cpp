#include "util.h"

char* compareShortOpt(char *opt, const ShortOpt *shortOpt)
{
	if (opt != nullptr && *opt == shortOpt->arg && *opt != '\0' && *opt != '=')
	{
		return ++opt;
	}
	return nullptr;
}


char* compareLongOpt(char *opt, const LongOpt *longOpt)
{
	if (opt == nullptr || *opt == '\0' || *opt == '=')
	{
		return nullptr;
	}

	char *argPtr = longOpt->arg;
	while (*opt != '\0' && *opt != '=' && *argPtr != '\0')
	{
		if (*opt != *argPtr)
		{
			return nullptr;
		}

		opt++;
		argPtr++;
	}

	if ((*opt == '\0' || *opt == '=') && *argPtr == '\0')
	{
		if (*opt == '=')
		{
			return ++opt;
		}
		return opt;
	}

	return nullptr;
}


char* getShortOpt(char *opt, const ShortOpt *shortOpts, int &id)
{
	while (shortOpts->arg != '\0')
	{
		char *result = compareShortOpt(opt, shortOpts);

		if (result != nullptr)
		{
			id = shortOpts->id;
			return result;
		}

		shortOpts++;
	}

	id = -1;
	return nullptr;
}


char* getLongOpt(char *opt, const LongOpt *longOpts, int &id)
{
	while (longOpts->arg != nullptr)
	{
		char *result = compareLongOpt(opt, longOpts);

		if (result != nullptr)
		{
			id = longOpts->id;
			return result;
		}

		longOpts++;
	}

	id = -1;
	return nullptr;
}


char* getOpt(char *arg, const ShortOpt *shortOpts, const LongOpt *longOpts, int &id)
{
	if (*arg == '-')
	{
		arg++;

		if (*arg == '-' && longOpts != nullptr)
		{
			arg++;
			return getLongOpt(arg, longOpts, id);
		}

		if (shortOpts != nullptr)
		{
			return getShortOpt(arg, shortOpts, id);
		}
	}

	id = -1;
	return nullptr;
}