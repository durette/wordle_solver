/*

Prerequisite:
Dictionary file stored in /usr/share/dict/words

Usage:
clear ; gcc -O2 -o wordle_solver wordle_solver.c -Wall -Wextra
./wordle_solver

It'll give you a word.
Enter the color codes for the results, where orange = O, black = B, green = G.
Example: OOBOB
Then press enter.
If the word is missing from their dictionary, type X to get a new guess.




*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define DICT_FILE "/usr/share/dict/words"
#define WORDLE_SIZE 5
#define MAX_DICTIONARY_SIZE 200000

struct DictionaryEntry {
   unsigned char spelling[WORDLE_SIZE];
   unsigned long int score;
};

void load_dictionary();
void compute_character_histogram();
void score_words();
void show_character_histogram();
void show_dictionary();
bool de_is_greater(struct DictionaryEntry a, struct DictionaryEntry b);
void swap_dictionary_entries(struct DictionaryEntry *a,
                             struct DictionaryEntry *b);
void heapify_dictionary_entries(struct DictionaryEntry arr[], long int n,
                                long int i);
void sort_dictionary();
void get_and_show_current_word();
void get_test_result();
void process_test_result();
struct DictionaryEntry dictionary[MAX_DICTIONARY_SIZE];
void filter_dictionary_by_green_result(unsigned long int green_letter_index);
void filter_dictionary_by_black_result(unsigned long int black_letter_index);
void filter_dictionary_by_orange_result(unsigned long int orange_letter_index);
void filter_dictionary_by_missing_result();

    struct DictionaryEntry current_word;
struct DictionaryEntry test_result;

unsigned long int dictionary_size = 0;
unsigned long int character_histogram[256];
bool won = false;

int main() {
   load_dictionary();
   compute_character_histogram();
   score_words();
   sort_dictionary();
   while (won == false) {
      get_and_show_current_word();
      get_test_result();
      process_test_result();
      sort_dictionary();
      // won = true;
   }
   return 0;
}

void load_dictionary() {
   FILE *file = fopen(DICT_FILE, "rb");
   // int characters_for_word[WORDLE_SIZE];
   unsigned long int file_column_index = 0;
   unsigned long int dictionary_index = 0;
   bool file_line_contains_junk = false;
   int file_character = getc(file);
   while (file_character != EOF) {
      if (file_character == '\n') {
         if (file_line_contains_junk == false && file_column_index == WORDLE_SIZE) {
            ++dictionary_index;
         } else {
            for (int letter_index = 0; letter_index < WORDLE_SIZE;
                 ++letter_index) {
               dictionary[dictionary_index].spelling[letter_index] = 0;
            }
         }
         file_line_contains_junk = false;
         file_column_index = 0;
      } else {
         if (file_column_index < WORDLE_SIZE) {
            if (file_character < 'A' || file_character > 'z' || (file_character > 'Z' && file_character < 'a')) {
               file_line_contains_junk = true;
            }
            if (file_character >= 'a' && file_character <= 'z') {
               file_character -= 'a' - 'A';
            }
            dictionary[dictionary_index].spelling[file_column_index] = file_character;
         }
         ++file_column_index;
      }
      file_character = getc(file);
   }
   fclose(file);
   dictionary_size = dictionary_index;
}

void compute_character_histogram() {
   unsigned long int letter_index = 0;
   for (int letter_index = 0; letter_index < 256; ++letter_index) {
      character_histogram[letter_index] = 0;
   }
   for (unsigned long int dictionary_index = 0; dictionary_index < dictionary_size; ++dictionary_index) {
      for (letter_index = 0; letter_index < WORDLE_SIZE; ++letter_index) {
         ++character_histogram[dictionary[dictionary_index].spelling[letter_index]];
      }
   }
}

void score_words() {
   for (unsigned long int dictionary_index = 0;
        dictionary_index < dictionary_size; ++dictionary_index) {
      for (unsigned long int letter_index = 0; letter_index < WORDLE_SIZE;
           ++letter_index) {
         unsigned long int this_letter_score = character_histogram[dictionary[dictionary_index].spelling[letter_index]];
         if (letter_index > 0) {
            for (unsigned long int duplicate_letter_index = 0; duplicate_letter_index < letter_index; ++duplicate_letter_index) {
               if (dictionary[dictionary_index].spelling[letter_index] == dictionary[dictionary_index].spelling[duplicate_letter_index]) {
                  this_letter_score = 0;
               }
            }
         }
         dictionary[dictionary_index].score += this_letter_score;
      }
   }
}

void show_character_histogram() {
   for (unsigned char i = 'A'; i <= 'Z'; ++i) {
      printf("%d (%c) = %ld\n", i, i, character_histogram[i]);
   }
}

void show_dictionary() {
   unsigned long int dictionary_index = 0;
   unsigned long int letter_index = 0;
   for (dictionary_index = 0; dictionary_index < dictionary_size;
        ++dictionary_index) {
      for (letter_index = 0; letter_index < WORDLE_SIZE; ++letter_index) {
         printf("%c", dictionary[dictionary_index].spelling[letter_index]);
      }
      printf(" = %ld\n", dictionary[dictionary_index].score);
   }
   printf("\ndictionary_size = %ld\n", dictionary_size);
}

bool de_is_greater(struct DictionaryEntry a, struct DictionaryEntry b) {
   if (a.score > b.score) {
      return true;
   }
   return false;
}

void swap_dictionary_entries(struct DictionaryEntry *a, struct DictionaryEntry *b) {
   struct DictionaryEntry temp = *a;
   *a = *b;
   *b = temp;
}

void heapify_dictionary_entries(struct DictionaryEntry arr[], long int n,
                                long int i) {
   int largest = i;
   int left = 2 * i + 1;
   int right = 2 * i + 2;

   if (left < n && !de_is_greater(arr[left], arr[largest]))
      largest = left;

   if (right < n && !de_is_greater(arr[right], arr[largest]))
      largest = right;

   // Swap and continue heapifying if root is not largest
   if (largest != i) {
      swap_dictionary_entries(&arr[i], &arr[largest]);
      heapify_dictionary_entries(arr, n, largest);
   }
}

void sort_dictionary() {
   for (long int i = dictionary_size / 2 - 1; i >= 0; i--) {
      heapify_dictionary_entries(dictionary, dictionary_size, i);
   }

   for (long int i = dictionary_size - 1; i >= 0; i--) {
      swap_dictionary_entries(&dictionary[0], &dictionary[i]);
      heapify_dictionary_entries(dictionary, i, 0);
   }
}

void get_and_show_current_word() {
   current_word = dictionary[0];
   for (unsigned long int li = 0; li < WORDLE_SIZE; ++li) {
      current_word.spelling[li] = dictionary[0].spelling[li];
      printf("%c", current_word.spelling[li]);
   }
   // printf(" score = %ld", current_word.score);
   printf("\n");
   // printf("\ndictionary_size = %ld\n", dictionary_size);
}

void get_test_result() {
   unsigned long int li = 0;
   for (li = 0; li < WORDLE_SIZE; ++li) {
      test_result.spelling[li] = 0;
   }
   printf("Enter the results from this test:\n");
   printf("G = green, O = orange, B = black, X = word missing from dictionary (skip), D = done\n");

   int input_char = getc(stdin);
   li = 0;
   while (input_char != EOF && input_char != '\n') {
      if (li < WORDLE_SIZE) {
         test_result.spelling[li] = input_char;
      }
      input_char = getc(stdin);
      ++li;
   }
}

void process_test_result() {
   for (unsigned long int li = 0; li < WORDLE_SIZE; ++li) {
      switch (test_result.spelling[li]) {
      case 'D':
      case 'd':
         exit(0);
      case 'G':
      case 'g':
         filter_dictionary_by_green_result(li);
         break;
      case 'B':
      case 'b':
         filter_dictionary_by_black_result(li);
         break;
      case 'O':
      case 'o':
      case 'Y': // accept yellow too
      case 'y':
         filter_dictionary_by_orange_result(li);
         break;
      case 'X':
      case 'x':
      case 'Z':
      case 'z':
         filter_dictionary_by_missing_result();
         break;
      default:
         break;
      }
   }
}

void filter_dictionary_by_green_result(unsigned long int green_letter_index) {
   // printf("Apply green rule for character %c in position %ld.\n", current_word.spelling[green_letter_index], green_letter_index);
   for (unsigned long int di = 0; di < dictionary_size; ++di) {
      if (dictionary[di].spelling[green_letter_index] != current_word.spelling[green_letter_index]) {
         dictionary[di].score = 0;
      }
   }
}

void filter_dictionary_by_black_result(unsigned long int black_letter_index) {
   // printf("Apply black rule for character %c in position %ld.\n", current_word.spelling[black_letter_index], black_letter_index);
   for (unsigned long int di = 0; di < dictionary_size; ++di) {
      for (unsigned long int li = 0; li < WORDLE_SIZE; ++li) {
         if (dictionary[di].spelling[li] == current_word.spelling[black_letter_index]) {
            dictionary[di].score = 0;
         }
      }
   }
}

void filter_dictionary_by_orange_result(unsigned long int orange_letter_index) {
   // printf("Apply orange rule for character %c in position %ld.\n", current_word.spelling[orange_letter_index], orange_letter_index);
   for (unsigned long int di = 0; di < dictionary_size; ++di) {
      if (dictionary[di].spelling[orange_letter_index] == current_word.spelling[orange_letter_index]) {
         dictionary[di].score = 0;
      }
   }
   for (unsigned long int di = 0; di < dictionary_size; ++di) {
      for (unsigned long int li = 0; li < WORDLE_SIZE; ++li) {
         if (li == orange_letter_index && dictionary[di].spelling[li] == current_word.spelling[orange_letter_index]) {
            dictionary[di].score = 0;
         }
      }
   }
}

void filter_dictionary_by_missing_result() {
   // printf("Apply missing rule for word.\n");
   dictionary[0].score = 0;
}
