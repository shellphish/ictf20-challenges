#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <inttypes.h>

#define SONGSIZE 256
#define SONGNAMESIZE 144
#define RANDSONGMOD 1000000000

const char *best_song = "\nYour inside is out\n"
                        "Your outside is in...\n"
                        "Everybody's got something to hide\n"
                        "Except for me and my monkey!\n\n";

void get_num(uint64_t *num) {
    scanf("%" SCNu64 , num);
    while (fgetc(stdin) != '\n') ; // Read until a newline is found
}

int make_unique_fname(char *fname) {
    snprintf(fname, SONGNAMESIZE, "/home/chall/service/append/song_%d", rand() % RANDSONGMOD);
    int len = strlen(fname);
    int counter = 1;

    if (access(fname, F_OK) != -1) {
        fname[len] = '0';
    }

    while (access(fname, F_OK) != -1) {
        if (counter == 10) {
            fname[len] = '0';
            len++;
            counter = 0;
            if (len+1 == SONGNAMESIZE) {
                return 0;
            }
        }
        fname[len] = '0' + counter++;
    }
    return 1;
}

void setup() {
    srand(time(0));
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

void welcome_message() {
    printf("\nWelcome to my song-making service!\n");
    printf("You are monkey #%lu\n", (unsigned long) &printf);
    printf("You can use this program to submit your garage-band hits directly to the big record labels.\n");
    printf("Don't doubt your vibe, let's see what you got!\n");
}

int print_menu() {
    uint64_t choice = 0;
    printf("\n---------------------------------------------\n");
    printf("What would you like to do, monkey?    \n");
    printf("(1) Write a song\n");
    printf("(2) Rate a song\n");
    printf("(3) View song\n");
    printf("(4) Exit\n");
    printf("> ");
    get_num(&choice);
    printf("\n");
    return choice;
}

void create_song() {
    char fname[SONGNAMESIZE] = {0};
    char confirm[2];
    int i = 0;
    uint64_t song_size = 0;

    printf("So you think you can make a hit?\n");
    printf("Give me your best lyrics! Better keep it short though...time is money!\n");
    printf("How long is your song? ");
    get_num(&song_size);
    char *lyrics = malloc(song_size);

    if (song_size > SONGSIZE) {
        printf("Get out of here, bonehead, I don't have time for this!\n");
        return;
    }

    printf("Alright, I'm listening.\n\n");
    while (i < (song_size & 0xFFFFFFFFFFFFFFF8) + 8 && read(0, &lyrics[i], 1) == 1) {
        if (lyrics[i] == '\n') {
            lyrics[i] = 0;
            break;
        }
        i++;
    }
    while (fgetc(stdin) != '\n') ; // Read until a newline is found

    printf("You sure you're ok with that? ");
    fgets(confirm, 2, stdin);
    if (!strcmp("n", confirm)) {
        printf("That's right, scram! You're gonna need thick skin if you want to survive here!\n");
        return;
    }

    lyrics[strcspn(lyrics, "\n")] = 0;
    if (!make_unique_fname(fname)) {
        printf("Can't make a file at this time; try again!\n");
        return;
    }

    FILE *fp = fopen(fname, "w");
    if (fp == NULL) {
        perror("Couldn't create this song! Something went very wrong :(\n");
        exit(1);
    }
    fwrite(lyrics, 1, song_size, fp);
    fclose(fp);
    printf("\nInteresting... it's got this ~hacker~ vibe to it.\n");
    printf("We'll call it... %s\nWe'll be in touch ;)\n", fname+32);
}

void rate_song() {
    char lyrics[SONGSIZE] = {0};
    uint64_t song_size = 0;

    printf("You want my advice? Alright, I'm a nice guy.\n");
    printf("Give me your best lyrics! Better keep it short though...\n");
    printf("How long is your song? ");
    get_num(&song_size);

    if (song_size < strlen(best_song)) {
        printf("A song that short will never do well!\n");
        return;
    }

    printf("Cool. How do the lyrics go?\n\n");
    fgets(lyrics, song_size, stdin);
    lyrics[strcspn(lyrics, "\n")] = 0;
    while (fgetc(stdin) != '\n') ; // Read until a newline is found

    printf("It lacks an element of ..");
    printf(". monkey.\nTry this for size:  \n\n");
    memcpy(lyrics, best_song, strlen(best_song));
    fwrite(lyrics, 1, song_size, stdout);
    printf("\n");
}

void view_song() {
    char lyrics[SONGSIZE] = {0};
    char fname[SONGNAMESIZE] = {0};

    printf("Which song number do you want to look at? ");
    sprintf(fname, "/home/chall/service/append/song_");
    fgets(fname + strlen(fname), SONGNAMESIZE-strlen(fname), stdin);
    fname[strcspn(fname, "\n")] = 0;

    if (access(fname, F_OK) == -1) {
        printf("Bzzzt. Couldn't find that song!\n");
        return;
    }

    FILE *fp = fopen(fname, "r");
    if (fp != NULL) {
        size_t newLen = fread(lyrics, sizeof(char), SONGSIZE, fp);
        if (ferror(fp) != 0) {
            fputs("Error reading file", stderr);
        }
        fclose(fp);
    } else {
        perror("Couldn't open the file! Something went very wrong :(\n");
        return;
    }
    printf("%s", lyrics);
}

int main() {
    setup();
    welcome_message();
    while (1) {
        int option = print_menu();
        if (option == 1) {
            create_song();
        } else if (option == 2) {
            rate_song();
        } else if (option == 3) {
            view_song();
        } else if (option == 4) {
            break;
        } else {
            printf("I didn't catch your vibe.\n");
        }
    }
    printf("Funding secured. Looks like you'll be a hit in no time!\n");
    return 0;
}
