#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

// Function to get the top 100 frequent words in a text file
unordered_map<string, double> getTopFrequentWords(const string& fileName) {
    unordered_map<string, int> wordCount;
    ifstream file(fileName);
    string word;

    // Read and normalize words
    while (file >> word) {
        // Convert to uppercase and remove any unwanted characters
        string normalizedWord;
        for (char ch : word) {
            if (isalnum(ch)) {
                normalizedWord += toupper(ch);
            }
        }

        // Exclude common words
        if (normalizedWord != "A" && normalizedWord != "AND" && normalizedWord != "AN" && 
            normalizedWord != "OF" && normalizedWord != "IN" && normalizedWord != "THE") {
            wordCount[normalizedWord]++;
        }
    }

    // Close the file
    file.close();

    // Sort words by frequency
    vector<pair<string, int>> sortedWords(wordCount.begin(), wordCount.end());
    sort(sortedWords.begin(), sortedWords.end(), 
         [](const pair<string, int>& a, const pair<string, int>& b) {
             return b.second < a.second;
         });

    // Take the top 100 frequent words
    unordered_map<string, double> frequentWords;
    int totalWords = 0;
    for (size_t i = 0; i < min<size_t>(100, sortedWords.size()); i++) {
        totalWords += sortedWords[i].second;
    }

    for (size_t i = 0; i < min<size_t>(100, sortedWords.size()); i++) {
        const auto& pair = sortedWords[i];
        const string& word = pair.first;
        int count = pair.second;
        frequentWords[word] = static_cast<double>(count) / totalWords;
    }

    return frequentWords;
}

// Function to calculate similarity between two word frequency maps
double calculateSimilarity(const unordered_map<string, double>& freqMap1, const unordered_map<string, double>& freqMap2) {
    double similarity = 0.0;

    for (const auto& pair : freqMap1) {
        const string& word = pair.first;
        double freq = pair.second;
        
        if (freqMap2.count(word)) {
            similarity += freq * freqMap2.at(word);
        }
    }

    return similarity;
}

int main() {
    const int numBooks = 64;
    vector<string> bookFiles(numBooks);

    // Assuming books are named book1.txt, book2.txt, ..., book64.txt
    for (int i = 0; i < numBooks; i++) {
        bookFiles[i] = "book" + to_string(i + 1) + ".txt";
    }

    // Compute top 100 frequent words for each book
    vector<unordered_map<string, double>> wordFrequencies(numBooks);
    for (int i = 0; i < numBooks; i++) {
        wordFrequencies[i] = getTopFrequentWords(bookFiles[i]);
    }

    // Calculate the similarity matrix
    vector<vector<double>> similarityMatrix(numBooks, vector<double>(numBooks, 0.0));
    for (int i = 0; i < numBooks; i++) {
        for (int j = i + 1; j < numBooks; j++) {
            double similarity = calculateSimilarity(wordFrequencies[i], wordFrequencies[j]);
            similarityMatrix[i][j] = similarity;
            similarityMatrix[j][i] = similarity; // Symmetric matrix
        }
    }

    // Store similarities in a vector for sorting and reporting top pairs
    vector<vector<double>> similarities;
    for (int i = 0; i < numBooks; i++) {
        for (int j = i + 1; j < numBooks; j++) {
            similarities.push_back({similarityMatrix[i][j], static_cast<double>(i), static_cast<double>(j)});
        }
    }

    // Sort similarities to find the top 10 pairs
    sort(similarities.begin(), similarities.end(), [](const vector<double>& a, const vector<double>& b) {
        return a[0] > b[0];
    });

    cout << "Top 10 similar pairs of books:\n";
    for (int k = 0; k < min(10, static_cast<int>(similarities.size())); k++) {
        double similarity = similarities[k][0];
        int i = static_cast<int>(similarities[k][1]);
        int j = static_cast<int>(similarities[k][2]);
        
        cout << "Book " << i + 1 << " and Book " << j + 1 << endl;
    }

    return 0;
}
