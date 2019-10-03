#include "main.h"

techi bsmeta<techi>::elements[] = {{"PlasmaScoring", "Плазменные заряды", Red},
{"MagenDefenceGrid", "Магнитная защитная сетка", Red, {1}},
{"DuraniumArmor", "Дюраниевая броня", Red, {2}},
{"AssaultCannon", "Осадная пушка", Red, {3}},
//
{"NeuralMotivator", "Невральный мотиватор", Green},
{"DacxiveAnimators", "Диаксивная анимация", Green, {0, 1}},
{"HyperMetabolism", "Хипер метаболизм", Green, {0, 2}},
{"X89BacterialWeapon", "Х89 бактериальное оружие", Green, {0, 3}},
//
{"AntimassDeflectors", "Анти-массовый отражатель", Blue},
{"GravityDriver", "Гравитационный двигатель", Blue, {0, 0, 1}},
{"FleetLogistics", "Ускоренное движение", Blue, {0, 0, 2}},
{"LightWaveDeflector", "Микроволновый отражатель", Blue, {0, 0, 3}},
};
assert_enum(tech, LastTech);