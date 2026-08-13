#include "Deck.hpp"
#include <random>
#include <iostream>

namespace {
	constexpr std::array<unsigned, 54> defaultCards = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53 };
	constexpr unsigned defaultRedJokerPosition = 52;
	constexpr unsigned defaultBlackJokerPosition = 53;
}

Deck::Deck(unsigned seed) : m_cards(defaultCards), m_redJokerPosition(defaultRedJokerPosition), m_blackJokerPosition(defaultBlackJokerPosition) {
	this->shuffleDeck(seed);
}

std::span<unsigned> Deck::getCards() {
	return this->m_cards;
}

unsigned Deck::getRedJokerPosition() const {
	return this->m_redJokerPosition;
}

void Deck::setRedJokerPosition(unsigned position) {
	this->m_redJokerPosition = position;
}

unsigned Deck::getBlackJokerPosition() const {
	return this->m_blackJokerPosition;
}

void Deck::setBlackJokerPosition(unsigned position) {
	this->m_blackJokerPosition = position;
}

void Deck::updateBlackJokerPosition() {
	// Find the position of the Black Joker in the deck and update the member variable if the position exists
	const auto it = std::find(this->m_cards.begin(), this->m_cards.end(), 53);
	if (it == this->m_cards.end()) {
		throw std::runtime_error("Black Joker not found in the deck.");
	}
	this->m_blackJokerPosition = static_cast<unsigned>(std::distance(this->m_cards.begin(), it));
}

void Deck::updateRedJokerPosition() {
	// Find the position of the Red Joker in the deck and update the member variable if the position exists
	const auto it = std::find(this->m_cards.begin(), this->m_cards.end(), 52);
	if (it == this->m_cards.end()) {
		throw std::runtime_error("Red Joker not found in the deck.");
	}
	this->m_redJokerPosition = static_cast<unsigned>(std::distance(this->m_cards.begin(), it));
}

void Deck::shuffleDeck(unsigned seed) {
	// Shuffle the indices using the provided seed
	std::mt19937 rng(seed);
	std::shuffle(this->m_cards.begin(), this->m_cards.end(), rng);

	// Find the positions of the Red Joker and Black Joker in the shuffled deck
	this->updateBlackJokerPosition();
	this->updateRedJokerPosition();
}

void Deck::resetDeck() {
	// Reset the deck to its default state
	this->m_cards = defaultCards;
	this->m_redJokerPosition = defaultRedJokerPosition;
	this->m_blackJokerPosition = defaultBlackJokerPosition;
}

std::ostream& operator<<(std::ostream& os, const Deck& deck) {
	// Create an array containing the representation of the cards in the deck, including the jokers
	static constexpr std::array<std::string_view, 54> cardRepresentations{
		"AT", "2T", "3T", "4T", "5T", "6T", "7T", "8T", "9T", "10T", "JT", "QT", "KT",
		"AC", "2C", "3C", "4C", "5C", "6C", "7C", "8C", "9C", "10C", "JC", "QC", "KC",
		"AH", "2H", "3H", "4H", "5H", "6H", "7H", "8H", "9H", "10H", "JH", "QH", "KH",
		"AP", "2P", "3P", "4P", "5P", "6P", "7P", "8P", "9P", "10P", "JP", "QP", "KP",
		"RedJoker", "BlackJoker"
	};
	// Get the cards and print them following their representations to the output stream
	std::span<const unsigned> cards = deck.m_cards;
	for (size_t i = 0; i < cards.size(); ++i) {
		os << cardRepresentations[cards[i]] << ' ';
	}
	os << '\n';
	os << "\nRed Joker Position : " << deck.m_redJokerPosition << '\n';
	os << "Black Joker Position : " << deck.m_blackJokerPosition << '\n';
	return os;
}
