#pragma once

#include "Deck.hpp"
#include <vector>

class SolitaireAlgorithm {
private:
	std::unique_ptr<Deck> m_deck;
public:
	SolitaireAlgorithm();
	SolitaireAlgorithm(unsigned seed);
	~SolitaireAlgorithm() = default;
	/* @brief Initialize the deck of cards and shuffle it using a seed provided by the user. If the user enters 0, a random seed will be generated using the current time as the seed for the random number generator.*/
	void initializeDeck(unsigned& seed);
	/* @brief Process the five Solitaire algorithm steps.
	 * @return The value of the output card, which will be used later to generate a letter.
	 */
	std::vector<unsigned char> getStreamKey(size_t messageLength, size_t alphabetSize);
};
