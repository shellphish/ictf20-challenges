#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <signal.h>
#include <curses.h>
#include <sys/mman.h>
#include <malloc.h>
#include <openssl/sha.h>

#define MY909 909
#define BANNER "1 AFTER 909: ASCII MUSIC LYRICS PREPARATION SERVICE\n"
#define FILE_PREFIX "1after909"
#define PREFIX_LENGTH sizeof(FILE_PREFIX)
#define USERNAME_PROMPT "Enter username: "
#define USERNAME_LENGTH 64
#define PASSWORD_PROMPT "Enter password: "
#define PASSWORD_LENGTH 64
#define REFERENCE_PROMPT "Enter document id: "
#define REFERENCE_LENGTH 64
#define TITLE_PROMPT "Enter title: "
#define TITLE_LENGTH 1024
#define TEXT_PROMPT "Enter text: "
#define TEXT_LENGTH 2048
#define PALETTE_PROMPT "Enter palette data\n"
#define PALETTE_LENGTH 2048
#define SEPARATOR_LENGTH 1
#define SEPARATOR "_"
#define PARAGRAPH_SEPARATOR "\n"
#define IMG_SEPARATOR "\n-------------------\n"
#define COMPOSITION_FILENAME_LENGTH PREFIX_LENGTH + SEPARATOR_LENGTH + USERNAME_LENGTH + SEPARATOR_LENGTH + PASSWORD_LENGTH + SEPARATOR_LENGTH + TITLE_LENGTH + 1
#define DATA_LENGTH_PROMPT "Length: "
#define DATA_PROMPT "Data: "
#define IMG_HEIGHT_PROMPT "Enter image height: "
#define IMG_WIDTH_PROMPT "Enter image width: "
#define IMG_PROMPT "Enter JPEG data\n"
#define IMG_LENGTH 1024 * 1024
#define CHUNK_SIZE 1024
#define COMMAND_PROMPT "Enter command: "
#define COMMAND_LENGTH 64
#define COMMAND_PALETTE "palette"
#define COMMAND_DOWNLOAD "print"
#define COMMAND_TEXT "text"
#define COMMAND_IMG "image"
#define COMMAND_QUIT "quit"
#define COMMAND_LIST "list"
#define BYEBYE "Bye!\n"
#define JPEG2ASCII_COMMAND "jp2a"
#define MAX_OUTPUT 10 * 1024 * 1024
#define TIMEOUT 0
#define HASH_LENGTH 80
/* #define HASHED_PASSWORD_OLD "04fa3ed8688b859a947f6e21b42dfc0b92704341a4b62d4a85a72b0d0c65a55a" */
/* "Shellphish is so 910" */
#define HASHED_PASSWORD "fc6b25dc07595b11f307d79cca713a332b784cbf2e52e6e30a372e7bd4935ebd"

#define ASCII_PALETTE_SIZE 2048
char ascii_palette[ASCII_PALETTE_SIZE + 1] = "   ...',;:clodxkO0KXNWM";

/* #define DEBUG */
/* #define DEBUG2 */
/* #define DEBUG3 */
/* #define DEBUG4 */
/* #define DEBUG5 */

int read_str(char *buf, int len)
{
	char c;
	int i = 0;

	while (1) {
		c = getchar();
		buf[i] = c;
		if ((i == len) || (c == '\0') || (c == '\n')) {
			buf[i] = '\0';
			break;
		}
		if (!isascii(c)) {
			buf[i] = '\0';
			return -1;
		}
		else {
			i++;
		}
	}
	return i;
}


int read_bin(char *buf, u_int maxlen)
{
	u_int i = 0;
	u_int len = 0;
	int res;

	printf(DATA_LENGTH_PROMPT);
	fflush(stdout);
	res = scanf("%d", &len);
	if (res != 1) {
		printf("Failed to read length.\n");
		fflush(stdout);
		return -1;
	}
	if (len > maxlen) {
		printf("Length exceeds capacity.\n");
		fflush(stdout);
		return -1;
	}
	printf(DATA_PROMPT);
	fflush(stdout);
	for (i = 0; i < len; i++) {
		res = read(0, &buf[i], 1);
		if (res < 1) {
			printf("Error while reading.\n");
			fflush(stdout);
			return  -1;
		}
	}
	return len;
}


char* convert(char* img, u_int img_len, char* palette, int width, int height)
{
	char* command[10];
	char palette_option[PALETTE_LENGTH];
	char width_option[TEXT_LENGTH];
	char height_option[TEXT_LENGTH];
	char* output = NULL;
	int fd1[2];
	int fd2[2];
	int pid = 0;

	command[0] = JPEG2ASCII_COMMAND;
	sprintf(width_option, "--width=%d", width);
	command[1] = width_option;
	sprintf(height_option, "--height=%d", height);
	command[2] = height_option;

	if (palette != NULL) {
		sprintf(palette_option, "--chars=\"%s\"", palette);
		command[3] = palette_option;
		command[4] = "-";
		command[5] = NULL;
	}
	else {
		command[3] = "-";
		command[4] = NULL;
	}

#ifdef DEBUG
	int l = 0;
	fprintf(stderr, "Input: [%s]\nLenght: %d\n", img, img_len);
	while (command[l] != NULL) {
		fprintf(stderr, "Command[%d]: %s\n", l, command[l]);
		l++;
	}
#endif

	if ((pipe(fd1) < 0) || (pipe(fd2) < 0)) {
        fprintf(stderr, "Cannot create pipes.\n");
        return NULL;
    }

#ifdef DEBUG
	fprintf(stderr, "Forking child...\n");
#endif

	if ((pid = fork()) < 0) {
        fprintf(stderr, "Cannot fork process.\n");
        return NULL;
    }

    /* Child */
    else if (pid == 0) {
#ifdef DEBUG
    	fprintf(stderr, "Executing child...\n");
#endif
		close(fd1[1]);
        close(fd2[0]);

        if (fd1[0] != STDIN_FILENO) {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO) {
     			fprintf(stderr, "Cannot duplicate input.\n");
        		exit(1);
            }
            close(fd1[0]);
        }

        if (fd2[1] != STDOUT_FILENO)
        {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO) {
                fprintf(stderr, "Cannot duplicate output.\n");
        		exit(1);
            }
            close(fd2[1]);
        }
#ifdef DEBUG
       	fprintf(stderr, "Executing command...\n");
#endif
        if (execvp(command[0], command) < 0 )
        {
            fprintf(stderr, "Cannot exec file.\n");
            exit(1);
        }
    }
    /* Parent */
    else {
		int rv;
		int i;

        close(fd1[0]);
        close(fd2[1]);

        /* This can be done in a more flexible way */
        output = malloc(MAX_OUTPUT);

#ifdef DEBUG
       	fprintf(stderr, "Executing parent...\n");
       	fprintf(stderr, "Writing to child...\n");
 #endif

        if (write(fd1[1], img, img_len) != img_len) {
        	fprintf(stderr, "Fail to write to pipe.\n");
     		return NULL;
        }
        close(fd1[1]);
        i = 0;
        while (1) {
#ifdef DEBUG
      		fprintf(stderr, "Reading from child...\n");
#endif
       		rv = read(fd2[0], &output[i], 1);
       		if (rv < 0) {
				fprintf(stderr, "Fail to read from pipe.\n");
     			return NULL;
        	}
        	else if (rv == 0) {
            	break;
        	}
        	i++;
        	if (i == MAX_OUTPUT -1) {
        		break;
        	}
        }
        output[i] = '\0';
    }

    return output;
}


void create_filename(char* buf, int maxlen, char* username, char* password, char* reference)
{
	snprintf(buf, maxlen, "%s%s%s%s%s%s%s",
		FILE_PREFIX,
		SEPARATOR,
		username,
		SEPARATOR,
		password,
		SEPARATOR,
		reference);

}


int verify_password(char *admin_password, char *hashed_password)
{
	char hash_string[(SHA256_DIGEST_LENGTH * 2) + 1];
	unsigned char *d = SHA256((unsigned char *)admin_password, strlen(admin_password), 0);
	int i;

	for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(&hash_string[i *2], "%02x", d[i]);
	}

	hash_string[SHA256_DIGEST_LENGTH * 2] = '\0';

#ifdef DEBUG2
	printf("password: %s\nhashed_password: %s\noriginal_hash  : %s\n", 
				admin_password,
				hash_string,
				hashed_password);
	fflush(stdout);
#endif

	if (strncmp(hashed_password, hash_string, SHA256_DIGEST_LENGTH * 2) != 0) {
		printf("Password does not match.\n");
		fflush(stdout);
		return 1;
	}

	return 0;
}

		
int doit()
{
	char username[USERNAME_LENGTH];
	char password[PASSWORD_LENGTH];
	char reference[REFERENCE_LENGTH];
	char title[TITLE_LENGTH];
	char composition_filename[COMPOSITION_FILENAME_LENGTH];
	FILE* file_handle = NULL;
	char text[TEXT_LENGTH];
	char img[IMG_LENGTH];
	char palette[PALETTE_LENGTH];
	int palette_len = 0;
	char *img_str = NULL;
	int img_len = 0;
	char command[COMMAND_LENGTH];
	char chunk[CHUNK_SIZE];
	int chunk_len = 0;
	int num_components = 0;
	int res;
	int pos;
	char c;
	char admin_password[PASSWORD_LENGTH];
	FILE *fd = NULL;
	int i;
	int width, height;
	struct data909 {
		char *pieces[MY909];
		char *hashed_password;
	};
	struct data909 data;

	strcpy(palette, ascii_palette);
	data.hashed_password = strdup(HASHED_PASSWORD);
	printf(BANNER);
	fflush(stdout);

	for (i = 0; i < MY909; i++) {
		data.pieces[1] = NULL;
	}

#ifdef DEBUG
	printf("Hashed password: %p Begin: %p End: %p Target: %p\n",
			data.hashed_password,
			&(data.pieces[0]),
			&(data.pieces[MY909 - 1]),
			&(data.pieces[MY909]));
#endif

	printf(USERNAME_PROMPT);
	fflush(stdout);
	res = read_str(username, USERNAME_LENGTH);
	if (res < 0) {
		printf("Failed to parse username.\n");
		fflush(stdout);
		return -1;
	}
	if (res == 0) {
		printf("String is empty.\n");
		fflush(stdout);
		return -1;
	}

	printf(PASSWORD_PROMPT);
	fflush(stdout);
	res = read_str(password, PASSWORD_LENGTH);
	if (res < 0) {
		printf("Failed to parse password.\n");
		fflush(stdout);
		return -1;
	}
	if (res == 0) {
		printf("String is empty.\n");
		fflush(stdout);
		return -1;
	}

	printf(REFERENCE_PROMPT);
	fflush(stdout);
	res = read_str(reference, REFERENCE_LENGTH);
	if (res < 0) {
		printf("Failed to parse document id.\n");
		fflush(stdout);
		return -1;
	}
#ifdef DEBUG
	printf("id len: %d\n", (unsigned)strlen(reference));
	fflush(stdout);
#endif
	if (strlen(reference) == 0) {
		pos = 0;
		while(1) {
			sprintf(reference, "%lx", (uintptr_t)(reference + pos));

			create_filename(composition_filename, COMPOSITION_FILENAME_LENGTH, username, password, reference);
			if (access(composition_filename, F_OK) != -1) {
				pos++;
			}
			else {
				break;
			}
		}
		printf("Created new document id: %s\n", reference);
		fflush(stdout);
		file_handle = fopen(composition_filename, "a+");
		if (file_handle == NULL) {
			printf("Cannot create document.\n");
			fflush(stdout);
			return -1;
		}
		printf(TITLE_PROMPT);
		fflush(stdout);
		res = read_str(title, TITLE_LENGTH);
		fprintf(file_handle, "%s\n---\n", title);
		fflush(file_handle);
	}
	else {
		create_filename(composition_filename, COMPOSITION_FILENAME_LENGTH, username, password, reference);
#ifdef DEBUG
		printf("Checking for doc: %s\n", composition_filename);
		fflush(stdout);
#endif
		if (access(composition_filename, F_OK) != 0) {
			printf("A document with the specified id does not exist.\n");
			fflush(stdout);
			sleep(1);
			return -1;
		}
#ifdef DEBUG
		printf("Accessing the file...\n");
		fflush(stdout);
#endif
		file_handle = fopen(composition_filename, "a+");
		if (file_handle == NULL) {
			printf("Cannot open document.\n");
			fflush(stdout);
			return -1;
		}
		printf("Title: ");
		fflush(stdout);
		fseek(file_handle, 0, SEEK_SET);
		while (1) {
			c = fgetc(file_handle);
			putchar(c);
			if (c == '\n') {
				break;
			}
			if (c == EOF) {
				printf("Empty title\n");
				fflush(stdout);
				return -1;
			}
		}
		fflush(stdout);
		fseek(file_handle, 0, SEEK_END);
	}

	num_components = 0;
	while (num_components <= MY909 + 1) {

#ifdef DEBUG5
		printf("%p",
				&(data.pieces));
		/* printf("%014d", num_components); */
#endif

#ifdef DEBUG4
		printf("n %d p %p t %p",
				num_components,
				&(data.pieces[num_components]),
				&(data.hashed_password));
		fflush(stdout);
#endif

#ifdef DEBUG3
		printf("Num components: %d, component address: %p, hashed password address: %p\n",
				num_components,
				&(data.pieces[num_components]),
				&(data.hashed_password));
#endif

		printf("\n%s", COMMAND_PROMPT);
		fflush(stdout);
		while (1) {
			/* Clear spurious characters */
			res = read_str(command, COMMAND_LENGTH);
			if (res > 0) {
				break;
			}
		}
		if (!strcmp(command, COMMAND_QUIT)) {
			break;
		}
		else if (!strcmp(command, COMMAND_LIST)) {
			printf("This command requires admin privileges. Please authenticate: ");
			fflush(stdout);
			res = read_str(admin_password, PASSWORD_LENGTH);
			if (res < 0) {
				printf("Error reading string.\n");
				break;
			}
			if (verify_password(admin_password, data.hashed_password) != 0) {
				printf("Credentials verification failed.\n");
				continue;
			}
			printf("Admin access granted.\n");
			fflush(stdout);
			fd = popen("/bin/ls", "r");
			while((c = fgetc(fd)) != EOF) {
				putchar(c);
			}
			fflush(stdout);
		}
		else if (!strcmp(command, COMMAND_TEXT)) {
			printf(TEXT_PROMPT);
			fflush(stdout);
			res = read_str(text, TEXT_LENGTH);
			if (res < 0) {
				printf("Error reading string.\n");
				break;
			}
			fwrite(text, 1, strlen(text), file_handle);
			fwrite(PARAGRAPH_SEPARATOR, 1, strlen(PARAGRAPH_SEPARATOR), file_handle);
			num_components++;
		}
		else if (!strcmp(command, COMMAND_IMG)) {
			printf(IMG_WIDTH_PROMPT);
			fflush(stdout);
			res = read_str(text, TEXT_LENGTH);
			if (res < 0) {
				printf("Error reading string.\n");
				break;
			}
			width = atoi(text);
			printf(IMG_HEIGHT_PROMPT);
			fflush(stdout);
			res = read_str(text, TEXT_LENGTH);
			if (res < 0) {
				printf("Error reading string.\n");
				break;
			}
			height = atoi(text);
			if ((width == 0) || (height == 0)) {
				printf("Illegal width or height\n");
				break;
			}
			printf(IMG_PROMPT);
			fflush(stdout);
			img_len = read_bin(img, IMG_LENGTH);
			if (img_len <= 0) {
				printf("Could not receive image.\n");
				break;
			}
			img_str = convert(img, img_len, palette, width, height);
			if (img_str == NULL) {
				printf("Image conversion failed.\n");
				break;
			}

			fwrite(IMG_SEPARATOR, 1, strlen(IMG_SEPARATOR), file_handle);
			fwrite(img_str, 1, strlen(img_str), file_handle);
			fwrite(IMG_SEPARATOR, 1, strlen(IMG_SEPARATOR), file_handle);
			data.pieces[num_components] = img_str;
#ifdef DEBUG
			printf("Image string is: %s\n", img_str);
			printf("Hash is now: %s\n", data.hashed_password);
			fflush(stdout);
#endif
			num_components++;
		}
		else if (!strcmp(command, COMMAND_DOWNLOAD)) {
			fseek(file_handle, 0, SEEK_SET);
			while(1) {
				chunk_len = fread(chunk, 1, CHUNK_SIZE, file_handle);
				fwrite(chunk, 1, chunk_len, stdout);
				if (chunk_len < CHUNK_SIZE) break;
			}
		}
		else if (!strcmp(command, COMMAND_PALETTE)) {
			printf(PALETTE_PROMPT);
			fflush(stdout);
			palette_len = read_bin(palette, PALETTE_LENGTH);
			if (palette_len <= 0) {
				printf("Failed to read palette.\n");
				break;
			}
			palette[palette_len] = '\0';
		}
		else {
			fflush(stdout);
		}
	}
	fflush(stdout);
	for (i = 0; i < MY909; i++) {
		if (data.pieces[1] != NULL) {
			free(data.pieces[1]);
		}
	}

	fclose(file_handle);
	printf(BYEBYE);
	fflush(stdout);
	return 0;
}


void timeout_handler(int signal)
{
  printf("\n\nTimeout expired!\n");
  fflush(stdout);
  sleep(1);
  exit(1);
}


int main(int argc, char** argv)
{
	int res;

	if (TIMEOUT != 0) {
		signal(SIGALRM, timeout_handler);
  		alarm(TIMEOUT);
  	}

	res = doit();

	sleep(1);
	exit(res);
}
