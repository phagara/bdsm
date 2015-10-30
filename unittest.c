#include <string.h>
#include <assert.h>
#include "buffer.h"
#include "bookstore.h"

int main(void) {
    printf("Initializing bookstore...\n");
    bookstore_t* store = bookstore_init();

    printf("Creating a book...\n");
    book_t* book = book_init("42", "MyBook", "Unknown", "all of em", 10, 1, 123.45);
    printf("Adding book to bookstore...\n");
    bookstore_add_book(store, book);
    printf("Printing bookstore...\n");
    bookstore_print(store);

    printf("Creating serialization buffer...\n");
    buffer_t* buf = buf_init();
    printf("Serializing bookstore...\n");
    serialize_bookstore(store, buf);
    printf("Freeing bookstore...\n");
    bookstore_free(store);
    printf("Printing buffer...\n");
    buf_print(buf);

    printf("Rewinding buffer...\n");
    buf_rewind(buf);
    printf("Unserializing bookstore from buffer...\n");
    store = unserialize_bookstore(buf);
    printf("Freeing buffer...\n");
    buf_free(buf);

    printf("Printing unserialized bookstore...\n");
    bookstore_print(store);

    printf("Getting first book from bookstore...\n");
    book = store->books[0];
    printf("Removing the book from bookstore...\n");
    bookstore_remove_book(store, book);
    printf("Freeing the removed book...\n");
    book_free(book);

    printf("Printing the (now empty) bookstore...\n");
    bookstore_print(store);

    printf("Initializing a new serialization buffer...\n");
    buf = buf_init();
    printf("Serializing bookstore into the buffer...\n");
    serialize_bookstore(store, buf);
    printf("Printing the buffer...\n");
    buf_print(buf);
    printf("Freeing the buffer...\n");
    buf_free(buf);

    printf("Adding 3 books into the bookstore...\n");
    bookstore_add_book(store, book_init("42", "MyBook", "Unknown", "all of em", 10, 1, 123.45));
    bookstore_add_book(store, book_init("43", "MyBook2", "Unknown", "some of em", 20, 11, 234.56));
    bookstore_add_book(store, book_init("44", "MyBook3", "Unknown", "some of em", 50, 2, 333.33));

    printf("Printing the bookstore...\n");
    bookstore_print(store);

    printf("Initializing a new serialization buffer...\n");
    buf = buf_init();
    printf("Serializing the bookstore into buffer...\n");
    serialize_bookstore(store, buf);
    printf("Printing the buffer...\n");
    buf_print(buf);
    printf("Freeing the buffer...\n");
    buf_free(buf);

    printf("Finding books by genre...\n");
    book = NULL;
    while ((book = books_by_genre(store, "some of em", book))) {
        printf("Found a match, changing its price...\n");
        book_change_price(book, 42);
        printf("...and selling 20 pcs...\n");
        assert(book_sell(book, 20));
        printf("...then trying to sell more than available...\n");
        assert(!book_sell(book, 9999));
        printf("...and printing the book...\n");
        book_print(book);
    }

    printf("Getting top 10 bestsellers (only 3 books available, though)...\n");
    bookstore_get_bestsellers(store, 10);
    printf("Listing sold-out titles...\n");
    bookstore_get_sold_out(store);

    printf("Saving bookstore to bookstore.dat...\n");
    bookstore_save(store, "bookstore.dat");

    printf("Freeing the bookstore...\n");
    bookstore_free(store);

    printf("Loading bookstore from file...\n");
    store = bookstore_load("bookstore.dat");

    printf("Printing the loaded bookstore...\n");
    bookstore_print(store);

    printf("Serializing bookstore and printing the resulting buffer...\n");
    buf = buf_init();
    serialize_bookstore(store, buf);
    buf_print(buf);
    buf_free(buf);

    printf("Freeing the bookstore...\n");
    bookstore_free(store);

    return 0;
}
