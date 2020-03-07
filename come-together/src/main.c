#include "httpd.h"
#include "session.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT "6666"

const char* const SIZES[] = {"250x250", "230x230", "60x60", "200x200"};
const char* const POSITIONS[] = {"+335+360", "+780+30", "+100+65", "+540+25"};

int main(int c, char **v) {
  setvbuf(stdout, NULL, _IONBF, 0);
  serve_forever(SERVER_PORT);
  return 0;
}

void ret_error(char * message){
  printf("HTTP/1.1 500 Internal Server Error\r\n");
  printf("Content-Type: text/html; charset=\"UTF-8\"\r\n");
  printf("Content-Length: %d\r\n\r\n", strlen(message));
  printf("%s\n", message);
}

void ret_redirect(char * uri){
  printf("HTTP/1.1 302 FOUND\r\n");
  printf("Location: %s\r\n\r\n", uri);
  printf("One thing I can tell you is\n");
}

void ret_template(char * t_name, char * param_1, char * param_2){
  filedata * data;
  int length;
  char filename[50];
  memset(filename, 0, 50);

  strcpy(filename, "templates/");
  strcat(filename, t_name);

  data = read_file(filename);

  if (data != NULL) {
    int param_1_len = 0, param_2_len = 0, tot_size = data->size, count = 0;

    // add param size and remove "%1$s" size
    if (param_1 != NULL){
      count = count_substr(data->buffer, "%1$s", data->size);
      param_1_len = strlen(param_1);
      tot_size += ((param_1_len - 4) * count);
    }

    if (param_2 != NULL){ 
      count = count_substr(data->buffer, "%2$s", data->size);
      param_2_len = strlen(param_2);
      tot_size += ((param_2_len - 4) * count);
    }

    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: text/html; charset=\"UTF-8\"\r\n");
    printf("Content-Length: %d\r\n\r\n", tot_size);
    printf(data->buffer, param_1, param_2);

    free(data->buffer);
    free(data);

  } else {
    ret_error("He got monkey finger"); 
  }
}

void ret_image(char * filename){
  filedata * data;

  data = read_file(filename);

  if (data != NULL) {
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: image/png\r\n");
    printf("Content-Length: %d\r\n\r\n", data->size);
    write(1, data->buffer, data->size);

    free(data->buffer);
    free(data);

  } else {
    ret_error("He got Ono sideboard");
  }
}

void ret_beatles_image(char *img_num){
  filedata * data;
  int length;
  char filename[50];
  memset(filename, 0, 50);

  strcpy(filename, "images/");
  strncat(filename, img_num, 10);
  strcat(filename, ".png");

  data = read_file(filename);

  if (data != NULL) {
    printf("HTTP/1.1 200 OK\r\n");
    printf("Content-Type: image/png\r\n");
    printf("Content-Length: %d\r\n\r\n", data->size);
    write(1, data->buffer, data->size);

    free(data->buffer);
    free(data);

  } else {
    ret_error("He shoot Coca-Cola");
  }
}

void route() {
  ROUTE_START()

  ROUTE_GET("/") {
    ret_template("index.html", NULL, NULL);
  }

  ROUTE_GET("/flag") {
    ret_redirect("https://www.youtube.com/watch?v=45cYwDMibGo");
  }

  ROUTE_GET_START("/amazing-beatles-images/") {
    char * image_num = uri + (strlen(uri) - 1);
    ret_beatles_image(image_num);
  }

  ROUTE_GET_START("/amazing-beatles-images-small/") {
    char image[3];
    image[0] = uri[strlen(uri) - 1];
    image[1] = 's';
    image[2] = '\0';
    ret_beatles_image(image);
  }

  ROUTE_GET("/a-beginning") {
    session * s = create_new_session();
    char redirect_uri[SESSION_SIZE + TOKEN_SIZE + 50];
    memset(redirect_uri, 0, SESSION_SIZE + TOKEN_SIZE + 50);

    if (s != NULL) {
      snprintf(redirect_uri, SESSION_SIZE + TOKEN_SIZE + 50, "/happiness?session=%s&token=%s", s->session_id, s->token);
      ret_redirect(redirect_uri);
    }
    else{
      ret_error("He say I know you, you know me");
    }

    free_session(s);
  }

  ROUTE_GET("/happiness") {
    session * s = get_session(qs);

    if (s == NULL){
      ret_error("You got to be free");
    }
    else {
      ret_template("new-session.html", s->session_id, s->token);
    }

    free_session(s);
  }

  ROUTE_POST_START("/peace") {
    char rname[13];
    char fname[22];
    char redirect_uri[SESSION_SIZE + TOKEN_SIZE + 50];
    memset(redirect_uri, 0, SESSION_SIZE + TOKEN_SIZE + 50);

    session * s = get_session(qs);

    if (s == NULL){
      ret_error("He bad production");
    }
    else {
      rand_string(rname, 13);
      snprintf(fname, 22, "data/tmp/%s", rname);
      write_file(fname, payload, payload_size);

      setenv("PEACE_FNAME", fname, 1337);
      setenv("SESSION_ID", s->session_id, 1337);
      int status = WEXITSTATUS(system("./parsepeace"));

      if (!status){
        snprintf(redirect_uri, SESSION_SIZE + TOKEN_SIZE + 50, "/dreamer?session=%s&token=%s", s->session_id, s->token);
        ret_redirect(redirect_uri);
      }
      else
        ret_error("He got walrus gumboot");
    }

    free_session(s);
  }

  ROUTE_GET("/love") {
    session * s = get_session(qs);

    if (s == NULL){
      ret_error("You can feel his disease");
    }
    else {
      char fname[94];
      strcpy(fname, "data/");
      strcat(fname, s->session_id);
      strcat(fname, "/img.png");
      strcat(fname, "\0");
      ret_image(fname);
    }

    free_session(s);
  }

  ROUTE_GET("/dreamer") {
    session * s = get_session(qs);

    if (s == NULL){
      ret_error("He say one and one and one is three");
    }
    else {
      ret_template("view-image.html", s->session_id, s->token);
    }

    free_session(s);
  }

  ROUTE_GET_START("/magic") {
    session * s = get_session(qs);

    if (s == NULL){
      ret_error("Got to be good looking");
    }
    else {
      char * img_id_str = strrchr(uri, '/');
      if (img_id_str == NULL){
        ret_error("'Cause he's so hard to see..");
      }
      // int img_id = atoi(uri + (strlen(uri) - 1)) - 1;
      // if (img_id < 0 || img_id > 3) {
      //   ret_error("'Cause he's so hard to see");
      // }
      else {
        int img_id = atoi(img_id_str + 1) - 1;
        setenv("IMG", uri + (strlen(uri) - 1), 1337);
        setenv("SIZE", SIZES[img_id], 1337);
        setenv("POSITION", POSITIONS[img_id], 1337);
        setenv("SESSION_ID", s->session_id, 1337);

        int status = WEXITSTATUS(system("./domagic"));

        if (!status){
          ret_template("magic.html", s->session_id, s->token);
        }
        else
          ret_error("Over me");
      }
    }

    free_session(s);
  }

  // ROUTE /wisdom

  ROUTE_END()
}
