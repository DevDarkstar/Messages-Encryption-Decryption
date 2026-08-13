#pragma once

#include <span>
#include <array>
#include <algorithm>
#include <iostream>

class Deck {
private:
	// Add private member variables and methods here
	// Cards of the deck are represented as integers from 0 to 53, where 52 is the Red Joker, 53 is the Black Joker, and 0-51 are the standard cards in order (AT, 2T, ..., KP)
	std::array<unsigned, 54> m_cards;
	// Position of the Red Joker in the deck
	unsigned m_redJokerPosition;
	// Position of the Black Joker in the deck
	unsigned m_blackJokerPosition;

public:
	/** Constructor for the Deck class that initializes the deck of cards and shuffles it using the provided seed
	* @param seed The seed used to shuffle the deck of cards
	*/
	Deck(unsigned seed);
	~Deck() = default;
	// Setter and getter for the cards vector
	std::span<unsigned> getCards();
	unsigned getRedJokerPosition() const;
	void setRedJokerPosition(unsigned position);
	unsigned getBlackJokerPosition() const;
	void setBlackJokerPosition(unsigned position);
	// Public methods
	void updateBlackJokerPosition();
	void updateRedJokerPosition();
	void shuffleDeck(unsigned seed);
	void resetDeck();
	// Operators overloading
	friend std::ostream& operator<<(std::ostream& os, const Deck& deck);
};
