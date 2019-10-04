#include "main.h"

static abstract_deck secret_objective;

void playeri::create_objectve_deck() {
	secret_objective.clear();
	for(auto i = FirstSecret; i <= LastSecret; i = (secret_s)(i + 1))
		secret_objective.add(i);
	secret_objective.shuffle();
}

void playeri::add_secret_objective(int value, bool interactive) {

}