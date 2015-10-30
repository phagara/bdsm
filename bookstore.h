#ifndef BOOKSTORE_H
#define BOOKSTORE_H
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
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


/* 
 * implementation
 */

book_t* book_init(const char* isbn, const char* title, const char* author,
        const char* genre, const unsigned int stocked_qty,
        const unsigned int sold_qty, const double price) {
    book_t* ret = malloc(sizeof(book_t));
    if (ret == NULL) exit(errno);
    ret->isbn = strdup(isbn);
    if (ret->isbn == NULL) exit(errno);
    ret->title = strdup(title);
    if (ret->title == NULL) exit(errno);
    ret->author = strdup(author);
    if (ret->author == NULL) exit(errno);
    ret->genre = strdup(genre);
    if (ret->genre == NULL) exit(errno);
    ret->stocked_qty = stocked_qty;
    ret->sold_qty = sold_qty;
    ret->price = price;
    return ret;
}

bookstore_t* bookstore_init(void) {
    bookstore_t* ret = malloc(sizeof(bookstore_t));
    if (ret == NULL) exit(errno);
    ret->num_books = 0;
    ret->books = NULL;
    return ret;
}

void serialize_book(const book_t* book, buffer_t* buf) {
    buf_write(buf, book->isbn, strlen(book->isbn) + 1);
    buf_write(buf, book->title, strlen(book->title) + 1);
    buf_write(buf, book->author, strlen(book->author) + 1);
    buf_write(buf, book->genre, strlen(book->genre) + 1);
    buf_write(buf, &(book->stocked_qty), sizeof(unsigned int));
    buf_write(buf, &(book->sold_qty), sizeof(unsigned int));
    buf_write(buf, &(book->price), sizeof(double));
}

book_t* unserialize_book(buffer_t* buf) {
    book_t* ret = malloc(sizeof(book_t));
    if (ret == NULL) exit(errno);
    ret->isbn = buf_readstr(buf);
    ret->title = buf_readstr(buf);
    ret->author = buf_readstr(buf);
    ret->genre = buf_readstr(buf);
    buf_readbytes(buf, &(ret->stocked_qty), sizeof(unsigned int));
    buf_readbytes(buf, &(ret->sold_qty), sizeof(unsigned int));
    buf_readbytes(buf, &(ret->price), sizeof(double));
    return ret;
}

void serialize_bookstore(const bookstore_t* store, buffer_t* buf) {
    buf_write(buf, &(store->num_books), sizeof(unsigned int));
    for (unsigned int i=0; i<store->num_books; i++)
        serialize_book(store->books[i], buf);
}

bookstore_t* unserialize_bookstore(buffer_t* buf) {
    bookstore_t* ret = bookstore_init();
    buf_readbytes(buf, &(ret->num_books), sizeof(unsigned int));
    ret->books = malloc(ret->num_books * sizeof(book_t*));
    if (ret->books == NULL) exit(errno);
    for (unsigned int i=0; i<ret->num_books; i++)
        ret->books[i] = unserialize_book(buf);
    return ret;
}

void bookstore_save(const bookstore_t* store, const char* filename) {
    buffer_t* buf = buf_init();
    serialize_bookstore(store, buf);

    FILE* fd = fopen(filename, "wb");
    if (fd == NULL) exit(errno);

    if (fwrite(buf->bytes, 1, buf->size, fd) != buf->size) {
        printf("Error saving bookstore!\n");
        fclose(fd);
        buf_free(buf);
        exit(1);
    }

    fclose(fd);
    buf_free(buf);
}

bookstore_t* bookstore_load(const char* filename) {
    if (access(filename, F_OK) == -1) {
        return NULL;
    }

    FILE* fd = fopen(filename, "rb");
    if (fd == NULL) exit(errno);

    fseek(fd, 0, SEEK_END);
    size_t file_len = ftell(fd);
    rewind(fd);

    buffer_t* buf = buf_init_from_fd(fd);
    fclose(fd);

    bookstore_t* ret = unserialize_bookstore(buf);
    buf_free(buf);
    return ret;
}

void bookstore_add_book(bookstore_t* store, book_t* book) {
    if (book_find(store, book->isbn) != NULL) {
        printf("Book with the same ISBN already exists in the bookstore!\n");
        return;
    }

    store->books = realloc(store->books, sizeof(book_t*) * (store->num_books + 1));
    if (store->books == NULL) exit(errno);
    store->books[store->num_books] = book;
    store->num_books++;
}

void bookstore_remove_book(bookstore_t* store, const book_t* book) {
    for (unsigned int i=0; i<store->num_books; i++) {
        if (store->books[i] == book) {
            memmove(&(store->books[i]), &(store->books[i+1]), store->num_books - i - 1);
            break;
        }
    }
    store->num_books--;
}

book_t* book_find(const bookstore_t* store, const char* isbn) {
    for (unsigned int i=0; i<store->num_books; i++) {
        if (strcmp(store->books[i]->isbn, isbn) == 0)
            return store->books[i];
    }

    return (book_t*) NULL;
}

book_t* books_by_author(const bookstore_t* store, const char* author, const book_t* last) {
    bool proceed = (last == NULL) ? true : false;

    for (unsigned int i=0; i<store->num_books; i++) {
        if (proceed && strcmp(store->books[i]->author, author) == 0) {
            return store->books[i];
        }

        if (!proceed && store->books[i] == last)
            proceed = true;
    }

    return (book_t*) NULL;
}

book_t* books_by_genre(const bookstore_t* store, const char* genre, const book_t* last) {
    bool proceed = (last == NULL) ? true : false;

    for (unsigned int i=0; i<store->num_books; i++) {
        if (proceed && strcmp(store->books[i]->genre, genre) == 0) {
            return store->books[i];
        }

        if (!proceed && store->books[i] == last)
            proceed = true;
    }

    return (book_t*) NULL;
}

bool book_sell(book_t* book, const unsigned int qty) {
    if (book->stocked_qty < qty) {
        printf("Stocked quantity is less than requested, cannot sell!\n");
        return false;
    } else {
        book->sold_qty += qty;
        book->stocked_qty -= qty;
        return true;
    }
}

void book_stock(book_t* book, const unsigned int qty) {
    book->stocked_qty += qty;
}

void book_change_price(book_t* book, const double price) {
    book->price = price;
}

void book_print(const book_t* book) {
    printf("(%s) `%s` by %s [genre=%s, stocked_qty=%u, sold_qty=%u, price=%.2f]\n",
            book->isbn, book->title, book->author, book->genre,
            book->stocked_qty, book->sold_qty, book->price);
}

void bookstore_print(const bookstore_t* store) {
    printf("Number of books: %u\n", store->num_books);
    for (unsigned int i=0; i<store->num_books; i++) book_print(store->books[i]);
}

void books_sort_by_sold_qty(book_t** books, const unsigned int num_books) {
    if (num_books < 2)
        return;

    book_t* p = books[num_books / 2];
    book_t* t;
    unsigned int i, j;
    for (i=0, j=num_books - 1;; i++, j--) {
        while (books[i]->sold_qty < p->sold_qty)
            i++;
        while (p->sold_qty < books[j]->sold_qty)
            j--;
        if (i >= j)
            break;
        t = books[i];
        books[i] = books[j];
        books[j] = t;
    }
    books_sort_by_sold_qty(books, i);
    books_sort_by_sold_qty(&books[i], num_books - i);
}

void bookstore_get_bestsellers(bookstore_t* store, unsigned int howmany) {
    if (howmany > store->num_books) {
        printf("Warning: you requested more bestsellers than there are books!\n");
        howmany = store->num_books;
    }
    books_sort_by_sold_qty(store->books, store->num_books);
    for (unsigned int i=0; i<howmany; i++) {
        book_print(store->books[store->num_books - i - 1]);
    }
}

void bookstore_get_sold_out(const bookstore_t* store) {
    for (unsigned int i=0; i<store->num_books; i++) {
        if (store->books[i]->stocked_qty == 0) {
            book_print(store->books[i]);
        }
    }
}

void book_free(book_t* book) {
    free(book->isbn);
    book->isbn = NULL;
    free(book->title);
    book->title = NULL;
    free(book->author);
    book->author = NULL;
    free(book->genre);
    book->genre = NULL;
    free(book);
    book = NULL;
}

void bookstore_free(bookstore_t* store) {
    for (unsigned int i=0; i<store->num_books; i++) book_free(store->books[i]);
    free(store->books);
    store->books = NULL;
    free(store);
    store = NULL;
}
#endif
