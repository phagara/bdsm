#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bookstore.h"

#define MAXCMDLEN 1024
#define MAXPARAMS 8

bool unsaved_changes = false;


bookstore_t* cmd_dispatch(bookstore_t* store, unsigned int argc, char** argv);
void bookshell(bookstore_t* store);


bookstore_t* cmd_dispatch(bookstore_t* store, unsigned int argc, char** argv) {
    if (strcmp(argv[0], "exit") == 0) {
        if (unsaved_changes) {
            printf("You have unsaved changes. Really exit? [yN] ");
            int choice = getchar();
            if (choice != 'y' && choice != 'Y' && choice != EOF)
                return store;
        }
        printf("Bye.\n");
        bookstore_free(store);
        exit(0);
    } else if (strcmp(argv[0], "help") == 0) {
        printf("List of available commands:\n");
        printf("\texit\n\t\texit the BDSM program\n");
        printf("\thelp\n\t\tthis text\n");
        printf("\tload <filename>\n\t\tloads a bookstore from file\n");
        printf("\tsave <filename>\n\t\tsaves bookstore to a file\n");
        printf("\treset\n\t\tre-initializes the bookstore\n");
        printf("\tbookadd <isbn> <title> <author> <genre> <stocked_qty> <sold_qty> <price>\n\t\tadds a new book to the bookstore\n");
        printf("\tbookdel <isbn>\n\t\tremoves a book from the bookstore\n");
        printf("\tbyauthor <author>\n\t\tfinds all books by author\n");
        printf("\tbygenre <genre>\n\t\tfind all books by genre\n");
        printf("\tsell <isbn> <qty>\n\t\tsells specified quantity of a book\n");
        printf("\tstock <isbn> <qty>\n\t\tadds specified quantity of a book to the stock\n");
        printf("\tchprice <isbn> <newprice>\n\t\tchanges price of a book\n");
        printf("\tinfo <isbn>\n\t\tshows details of a book\n");
        printf("\tls\n\t\tlists all books in the bookstore\n");
        printf("\ttop <N>\n\t\tlists top N bestsellers\n");
        printf("\tsoldout\n\t\tlists all sold-out books\n");
        printf("\trevenue\n\t\tprints number of books sold and their total price\n");
        return store;
    } else if (strcmp(argv[0], "load") == 0) {
        if (argc <= 1) {
            printf("The \"load\" command requires a filename as a parameter\n");
            return store;
        }
        if (unsaved_changes) {
            printf("You have unsaved changes. Really load a new bookstore? [yN] ");
            int choice = getchar();
            if (choice == EOF) {
                printf("Bye.\n");
                bookstore_free(store);
                exit(0);
            }
            if (choice != 'y' && choice != 'Y')
                return store;
        }
        bookstore_t* newstore;
        if ((newstore = bookstore_load(argv[1])) != NULL) {
            bookstore_free(store);
            store = newstore;
            unsaved_changes = false;
            printf("Loaded bookstore from %s\n", argv[1]);
        } else {
            printf("Failed to load bookstore from %s!\n", argv[1]);
        }
        return store;
    } else if (strcmp(argv[0], "save") == 0) {
        if (argc <= 1) {
            printf("The \"save\" command requires a filename as a parameter\n");
            return store;
        }
        bookstore_save(store, argv[1]);
        unsaved_changes = false;
        return store;
    } else if (strcmp(argv[0], "reset") == 0) {
        bookstore_free(store);
        unsaved_changes = true;
        return bookstore_init();
    } else if (strcmp(argv[0], "bookadd") == 0) {
        if (argc <= 7) {
            printf("The \"bookadd\" command requires book details as parameters (see \"help\" for details)\n");
            return store;
        }
        bookstore_add_book(store,
                book_init(argv[1], argv[2], argv[3],
                    argv[4], (unsigned int) atoi(argv[5]),
                    (unsigned int) atoi(argv[6]), atof(argv[7])));
        unsaved_changes = true;
        return store;
    } else if (strcmp(argv[0], "bookdel") == 0) {
        if (argc <= 1) {
            printf("The \"bookdel\" command requires book ISBN as a pameter\n");
            return store;
        }
        book_t* b = book_find(store, argv[1]);
        if (b != NULL) {
            bookstore_remove_book(store, b);
            book_free(b);
            unsaved_changes = true;
        } else {
            printf("Cannot find book with ISBN %s!\n", argv[1]);
        }
        return store;
    } else if (strcmp(argv[0], "byauthor") == 0) {
        if (argc <= 1) {
            printf("The \"byauthor\" command requires an author name as a pameter\n");
            return store;
        }
        book_t* b = NULL;
        while ((b = books_by_author(store, argv[1], b)) != NULL)
            book_print(b);
        return store;
    } else if (strcmp(argv[0], "bygenre") == 0) {
        if (argc <= 1) {
            printf("The \"bygenre\" command requires a genre as a pameter\n");
            return store;
        }
        book_t* b = NULL;
        while ((b = books_by_genre(store, argv[1], b)) != NULL)
            book_print(b);
        return store;
    } else if (strcmp(argv[0], "sell") == 0) {
        if (argc <= 2) {
            printf("The \"sell\" command requires book ISBN and quantity as pameters\n");
            return store;
        }
        book_t* b = book_find(store, argv[1]);
        if (b != NULL) {
            book_sell(b, (unsigned int) atoi(argv[2]));
            unsaved_changes = true;
        } else {
            printf("Cannot find book with ISBN %s!\n", argv[1]);
        }
        return store;
    } else if (strcmp(argv[0], "stock") == 0) {
        if (argc <= 2) {
            printf("The \"stock\" command requires book ISBN and quantity as pameters\n");
            return store;
        }
        book_t* b = book_find(store, argv[1]);
        if (b != NULL) {
            book_stock(b, (unsigned int) atoi(argv[2]));
            unsaved_changes = true;
        } else {
            printf("Cannot find book with ISBN %s!\n", argv[1]);
        }
        return store;
    } else if (strcmp(argv[0], "chprice") == 0) {
        if (argc <= 2) {
            printf("The \"chprice\" command requires book ISBN and new price as pameters\n");
            return store;
        }
        book_t* b = book_find(store, argv[1]);
        if (b != NULL) {
            book_change_price(b, atof(argv[2]));
            unsaved_changes = true;
        } else {
            printf("Cannot find book with ISBN %s!\n", argv[1]);
        }
        return store;
    } else if (strcmp(argv[0], "info") == 0) {
        if (argc <= 1) {
            printf("The \"info\" command requires book ISBN as a pameter\n");
            return store;
        }
        book_t* b = book_find(store, argv[1]);
        if (b != NULL)
            book_print(b);
        else
            printf("Cannot find book with ISBN %s!\n", argv[1]);
        return store;
    } else if (strcmp(argv[0], "ls") == 0) {
        bookstore_print(store);
        return store;
    } else if (strcmp(argv[0], "top") == 0) {
        if (argc <= 1) {
            printf("The \"top\" command requires a number as a parameter\n");
            return store;
        }
        bookstore_get_bestsellers(store, (unsigned int) atoi(argv[1]));
        return store;
    } else if (strcmp(argv[0], "soldout") == 0) {
        bookstore_get_sold_out(store);
        return store;
    } else if (strcmp(argv[0], "revenue") == 0) {
        unsigned int n = 0;
        double sum = 0;
        for (unsigned int i=0; i<store->num_books; i++) {
            n += store->books[i]->sold_qty;
            sum += store->books[i]->price * store->books[i]->sold_qty;
        }
        printf("Total %d books sold, totaling %.02f $currency\n", n, sum);
        return store;
    }

    printf("Unknown command: %s\n", argv[0]);
    printf("Try \"help\" to get a list of available commands\n");
    return store;
}


void bookshell(bookstore_t* store) {
    char cmd[MAXCMDLEN];
    char* params[MAXPARAMS];
    unsigned int i;
    char* last;

    printf("\nBDSM v1.3.37 shell ready.\n");
    printf("Type \"help\" for instructions.\n");

    while (true) {
        printf("> ");

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            printf("Bye.\n");
            bookstore_free(store);
            exit(0);
        }

        if (cmd[strlen(cmd)-1] == '\n')
            cmd[strlen(cmd)-1] = '\0';

        params[0] = strtok_r(cmd, " \t", &last);
        if (params[0] != NULL) {
            for (i=1; i<MAXPARAMS; i++) {
                 params[i] = strtok_r(NULL, " \t", &last);
                 if (params[i] == NULL)
                     break;
            }

            store = cmd_dispatch(store, i, params);
        }
    }
}


int main(int argc, char** argv) {
    printf("  ____________________________________________\n");
    printf(" /                                            \\\n");
    printf("| Bookstore Database and Sales Manager v1.3.37 |\n");
    printf(" \\____________________________________________/\n\n");

    bookstore_t* store;

    if (argc == 1) {
        printf("NOTE: No filename specified, working in-memory only.\n");
        printf("HINT: To load and work with a file-based bookstore database, use:\n");
        printf("\t%s <filename>\n", argv[0]);
        printf("...or simply type \"load <filename>\". Make sure to save often!\n");
        store = bookstore_init();
    } else if (argc == 2) {
        if ((store = bookstore_load(argv[1])) != NULL) {
            printf("Loaded bookstore database from %s...\n", argv[1]);
        } else {
            printf("Bookstore database file %s does not exist yet, creating...\n", argv[1]);
            store = bookstore_init();
            // or we just didn't have read permission,
            // in which case the following will terminate the program
            bookstore_save(store, argv[1]);
        }
    } else {
        printf("ERROR: Too many arguments!\n");
        printf("Usage:\n");
        printf("\t%s [filename]\n", argv[0]);
        exit(1);
    }

    bookshell(store);

    return 0;
}
