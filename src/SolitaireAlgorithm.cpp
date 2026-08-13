#include "SolitaireAlgorithm.hpp"
#include <chrono>

namespace {
	//seed to test for first step : 9 (blackJoker at last position)
	/* @brief First step of the Solitaire algorithm : move the black joker down one position in the deck.
	* If the black joker is in the last position, move it to the second position of the deck.
	*/
	void firstStep(Deck& deck) {
		// Get the position of the black joker in the deck.
		unsigned blackJokerPosition = deck.getBlackJokerPosition();
		// and the deck of cards.
		std::span<unsigned> cards = deck.getCards();

		// If the black joker is in the last position, move it to the second position of the deck.
		if (blackJokerPosition == cards.size() - 1) {
			std::rotate(cards.begin() + 1, cards.begin() + blackJokerPosition, cards.end());
			// Update the position of the black joker in the deck.
			deck.setBlackJokerPosition(1);
		}
		// Else swap the black joker with the card in the next position.
		else {
			std::swap(cards[blackJokerPosition], cards[static_cast<size_t>(blackJokerPosition) + 1]);
			// Update the position of the black joker in the deck.
			deck.setBlackJokerPosition(blackJokerPosition + 1);
		}
		// Update the position of the red joker in the deck.
		deck.updateRedJokerPosition();

		// Display the deck after the first step of the algorithm.
		//std::cout << "Deck after first step : " << deck << '\n';
	}

	// seed to test for second step : 24 (redJoker at last position after first step) and 27 (redJoker at penultimate position after first step)
	/* @brief Second step of the Solitaire algorithm : move the red joker down two positions in the deck.
	* If the red joker is in the last position, move it to the third position of the deck.
	* If the red joker is in the penultimate position, move it to the second position of the deck.
	*/
	void secondStep(Deck& deck) {
		// Get the position of the red joker in the deck.
		unsigned redJokerPosition = deck.getRedJokerPosition();
		// and the deck of cards.
		std::span<unsigned> cards = deck.getCards();

		// If the red joker is in the last position, move it to the third position of the deck.
		if (redJokerPosition == cards.size() - 1) {
			std::rotate(cards.begin() + 2, cards.begin() + redJokerPosition, cards.end());
			// Update the position of the red joker in the deck.
			deck.setRedJokerPosition(2);
		}
		// Else if the red joker is in the penultimate position, move it to the second position of the deck.
		else if (redJokerPosition == cards.size() - 2) {
			std::rotate(cards.begin() + 1, cards.begin() + redJokerPosition, cards.begin() + redJokerPosition + 1);
			// Update the position of the red joker in the deck.
			deck.setRedJokerPosition(1);
		}
		// Else move back the red joker by two positions in the deck.
		else {
			std::rotate(cards.begin() + redJokerPosition, cards.begin() + redJokerPosition + 1, cards.begin() + redJokerPosition + 3);
			// Update the position of the red joker in the deck.
			deck.setRedJokerPosition(redJokerPosition + 2);
		}
		// Update the position of the black joker in the deck.
		deck.updateBlackJokerPosition();

		// Display the deck after the second step of the algorithm.
		//std::cout << "Deck after second step : " << deck << '\n';
	}

	/* @brief Third step of the Solitaire algorithm : perform a double cut on the deck.
	* The purpose is to switch the cards above the first joker with the cards below the second joker, while keeping the jokers and the cards between them in the same order.
	*/
	void thirdStep(Deck& deck) {
		// Get the positions of the jokers in the deck.
		int redJokerPosition = deck.getRedJokerPosition();
		int blackJokerPosition = deck.getBlackJokerPosition();
		// Get the sorted positions of the jokers in the deck, independent of their order
		const auto [firstJokerPosition, lastJokerPosition] = std::minmax(redJokerPosition, blackJokerPosition);
		// and the deck of cards.
		std::span<unsigned> cards = deck.getCards();

		// Then perform a double cut on the deck, switching the cards above the first joker with the cards below the second joker, while keeping the jokers and the cards between them in the same order.
		std::rotate(cards.begin() + firstJokerPosition, cards.begin() + lastJokerPosition + 1, cards.end());
		std::rotate(cards.begin(), cards.begin() + firstJokerPosition, cards.end());

		// Update the positions of the jokers in the deck.
		deck.updateRedJokerPosition();
		deck.updateBlackJokerPosition();

		// Display the deck after the third step of the algorithm.
		//std::cout << "Deck after third step : " << deck << '\n';
	}

	/* @brief Fourth step of the Solitaire algorithm : perform a count cut on the deck.
	* The purpose is to cut the deck based on the value of the last card of the deck.
	*/
	void fourthStep(Deck& deck) {
		// Get the deck of cards.
		std::span<unsigned> cards = deck.getCards();
		// Get the value of the last card in the deck based on the Bridge order (both joker have the same value of 53).
		unsigned lastCard = static_cast<unsigned>(std::min(static_cast<int>(cards.back()) + 1, 53));
		//std::cout << "Last card value for count cut : " << lastCard << '\n';

		// put the 'lastCard value' cards from the top of the deck just above the last card of the deck.
		std::rotate(cards.begin(), cards.begin() + lastCard, cards.end() - 1);
		// Update the positions of the jokers in the deck.
		deck.updateRedJokerPosition();
		deck.updateBlackJokerPosition();
		// Display the deck after the fourth step of the algorithm.
		//std::cout << "Deck after fourth step : " << deck << '\n';
	}

	// seed to test for fifth step : 21 (output card is a joker)
	/* @brief Fifth step of the Solitaire algorithm : get a letter based on the value of the output card.
	* The purpose is to get the value of the first card of the deck.
	* Then move down the deck by its value, and return the value of the card at that position.
	* @return The value of the output card, which will be used later to generate a letter. If the chosen card is a joker , return -1.
	*/
	int fifthStep(Deck& deck) {
		// Get the deck of cards.
		std::span<unsigned> cards = deck.getCards();
		// Get the value of the first card in the deck based on the Bridge order (both joker have the same value of 53).
		unsigned firstCard = static_cast<unsigned>(std::min(static_cast<int>(cards.front()) + 1, 53));
		//std::cout << "First card value for output card : " << firstCard << '\n';
		// Move down the deck by its value, and get the value of the card at that position.
		unsigned outputCard = cards[firstCard];
		//std::cout << "Output card value : " << outputCard << '\n';
		// If the chosen card is a joker, return -1.
		if (outputCard == 52 || outputCard == 53) {
			return -1;
		}
		return outputCard;
	}

	/* @brief Check if the seed provided by the user is a valid positive integer. If the seed is valid, it will be parsed and stored in the parsedSeed variable.
	* @param seed The seed provided by the user as a string_view
	* @param parsedSeed The variable in which the parsed seed will be stored if it is valid
	* @return true if the seed is valid, false otherwise
	*/
	bool isSeedValid(std::string_view seed, unsigned& parsedSeed) {
		// Try to parse the seed as an unsigned integer.
		auto [ptr, ec] = std::from_chars(seed.data(), seed.data() + seed.size(), parsedSeed);

		// If the seed is valid, return true, otherwise false.
		if (ec == std::errc())
			return true;
		// If the seed is invalid, print an error message to the user.
		else if (ec == std::errc::invalid_argument)
			std::cout << "The current input is not a valid positive integer.\n";
		else if (ec == std::errc::result_out_of_range)
			std::cout << "This number is larger than an integer.\n";
		return false;
	}
}

SolitaireAlgorithm::SolitaireAlgorithm() : m_deck{ nullptr } {}

SolitaireAlgorithm::SolitaireAlgorithm(unsigned seed) : m_deck{ std::make_unique<Deck>(seed) } {}

void SolitaireAlgorithm::initializeDeck(unsigned& seed) {
	// Ask the user to select a seed for the deck. The seed must be a positive integer. If the user enters an invalid seed, the program will ask for a new seed until a valid one is provided.
	// If the user chose a seed of 0, the program will generate a random seed using the current time as the seed for the random number generator.
	if (seed == 0) {
		auto now = std::chrono::high_resolution_clock::now();
		seed = static_cast<unsigned>(now.time_since_epoch().count());
	}
		// Create a new deck with the provided seed.
		this->m_deck = std::make_unique<Deck>(seed);
}

std::vector<unsigned char> SolitaireAlgorithm::getStreamKey(size_t messageLength, size_t alphabetSize) {
	// Set the number of possibilities for the output cards
	static constexpr size_t cardRange = 52;
	// Set the maximum limit for the output cards product based on the alphabet size. Because the alphabet size is currently 256 and the cardRange is 52,
	// the purpose is to execute the algorithm two times for each character of the message, and then multiply the two output cards values to get a number between 0 and 2703 (52*52-1), which is then used to get a character from the alphabet.
	// The maxLimit is here to ensure that the output cards product makes each character of the alphabet has the same probability of being chosen, and to avoid bias in the output stream key.
	static const size_t maxLimit = cardRange * cardRange / alphabetSize * alphabetSize - 1;
	std::vector<unsigned char> streamKey;
	streamKey.reserve(messageLength);

	for (size_t i = 0; i < messageLength; ++i) {
		size_t outputCardsProduct{};
		std::array<size_t, 2> outputCards{};
		do {
			for (int cardIndex = 0; cardIndex < 2; ++cardIndex) {
				int outputCard = -1;
				// Process the algorithm steps in order.
				do {
					firstStep(*this->m_deck);
					secondStep(*this->m_deck);
					thirdStep(*this->m_deck);
					fourthStep(*this->m_deck);
					outputCard = fifthStep(*this->m_deck);
				} while (outputCard == -1);
				// Store the correct output card in the outputCards array.
				outputCards[cardIndex] = outputCard;
			}
			// Calculate the product of the two output cards to get a number between 0 and 2703 (52*52-1), then test the limit in order to ensure uniform distribution.
			outputCardsProduct = outputCards[0] * cardRange + outputCards[1];
		} while (outputCardsProduct > maxLimit);
		streamKey.push_back(static_cast<unsigned char>(outputCardsProduct % alphabetSize));
	}
	return streamKey;
}
