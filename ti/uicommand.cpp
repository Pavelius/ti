#include "main.h"

static struct command_info {
	const char*		name;
} command_data[] = {{""},
{"Принять"},
{"Да"},
{"Нет"},
{"Отмена"},
};
getstr_enum(command);