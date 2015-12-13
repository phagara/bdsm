#ifndef BOOKSTORE_H
#define BOOKSTORE_H
#include <stdbool.h>
#include "buffer.h"

/*
 * structs
 */

typedef struct book_struct {
    char* isbn;
    char* title;
    char* author;
    char* genre;
    unsigned int stocked_qty;
    unsigned int sold_qty;
    double price;
} book_t;

typedef struct bookstore_struct {
    unsigned int num_books;
    book_t** books;
} bookstore_t;


/*
 * function prototypes
 */

// creates a new book 
book_t* book_init(const char* isbn, const char* title, const char* author,
        const char* genre, const unsigned int stocked_qty,
        const unsigned int sold_qty, const double price);

// creates a new, empty bookstore
bookstore_t* bookstore_init(void);

// serializes book into a buffer
void serialize_book(const book_t* book, buffer_t* buf);

// unserializes a book from a buffer
book_t* unserialize_book(buffer_t* buf);

// serializes bookstore into a buffer
void serialize_bookstore(const bookstore_t* store, buffer_t* buf);

// unserializes a bookstore from a buffer
bookstore_t* unserialize_bookstore(buffer_t* buf);

// writes bookstore into a file
void bookstore_save(const bookstore_t* store, const char* filename);

// reads bookstore from a file
bookstore_t* bookstore_load(const char* filename);

// adds book into a bookstore
void bookstore_add_book(bookstore_t* store, book_t* book);

// removes book from a bookstore
void bookstore_remove_book(bookstore_t* store, const book_t* book);

// finds a book by its ISBN
book_t* book_find(const bookstore_t* store, const char* isbn);

// iterates through a bookstore, returning books written by the given author
book_t* books_by_author(const bookstore_t* store, const char* author, const book_t* last);

// iterates through a bookstore, returning books having the given genre
book_t* books_by_genre(const bookstore_t* store, const char* genre, const book_t* last);

// sells a given quantity of a book
bool book_sell(book_t* book, const unsigned int qty);

// adds a given quantity of a book to stock
void book_stock(book_t* book, const unsigned int qty);

// changes the price of a book
void book_change_price(book_t* book, const double price);

// prints the book's details
void book_print(const book_t* book);

// prints the number of books in store, plus details of all the books
void bookstore_print(const bookstore_t* store);

// sorts an awway of books by their sold quantity, ascending
void books_sort_by_sold_qty(book_t** books, const unsigned int num_books);

// prints top N bestsellers from the bookstore
void bookstore_get_bestsellers(bookstore_t* store, unsigned int howmany);

// prints sold-out books in the bookstore
void bookstore_get_sold_out(const bookstore_t* store);

// releases the memory allocated to a book
void book_free(book_t* book);

// releases the memory allocated to a bookstore and all the books in it
void bookstore_free(bookstore_t* store);

#endif
