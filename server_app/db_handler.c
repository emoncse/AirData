#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db_handler.h"
#include "logger.h"

#define DB_HOST "localhost"
#define DB_USER "imran"
#define DB_PASS "Emon@123"
#define DB_NAME "user_data"

MYSQL *connect_to_database();
int parse_user_data(const char *user_data, int *id, char *first_name, char *last_name, char *email, char *city);
void sanitize_input(MYSQL *conn, char *safe_email, char *safe_city, const char *email, const char *city);
int execute_insert_query(MYSQL *conn, int id, const char *first_name, const char *last_name, const char *safe_email, const char *safe_city);

int store_user_data(const char *user_data) {
    MYSQL *conn = connect_to_database();
    if (!conn) return -1;

    int id;
    char first_name[50], last_name[50], email[100], city[50];
    if (parse_user_data(user_data, &id, first_name, last_name, email, city) == -1) {
        mysql_close(conn);
        return -1;
    }

    char safe_email[200], safe_city[100];
    sanitize_input(conn, safe_email, safe_city, email, city);

    int result = execute_insert_query(conn, id, first_name, last_name, safe_email, safe_city);
    mysql_close(conn);
    return result;
}

MYSQL *connect_to_database() {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        log_error("MySQL initialization failed");
        return NULL;
    }

    if (!mysql_real_connect(conn, DB_HOST, DB_USER, DB_PASS, DB_NAME, 0, NULL, 0)) {
        log_error("MySQL connection failed: %s", mysql_error(conn));
        return NULL;
    }

    return conn;
}

int parse_user_data(const char *user_data, int *id, char *first_name, char *last_name, char *email, char *city) {
    if (sscanf(user_data, "%d,%49[^,],%49[^,],%99[^,],%49[^\n]", id, first_name, last_name, email, city) != 5) {
        log_error("Invalid data format: %s", user_data);
        return -1;
    }
    return 0;
}

void sanitize_input(MYSQL *conn, char *safe_email, char *safe_city, const char *email, const char *city) {
    mysql_real_escape_string(conn, safe_email, email, strlen(email));
    mysql_real_escape_string(conn, safe_city, city, strlen(city));
}

int execute_insert_query(MYSQL *conn, int id, const char *first_name, const char *last_name, const char *safe_email, const char *safe_city) {
    char query[1024];
    snprintf(query, sizeof(query),
        "INSERT INTO users (id, first_name, last_name, email, city) VALUES (%d, '%s', '%s', '%s', '%s') "
        "ON DUPLICATE KEY UPDATE first_name='%s', last_name='%s', email='%s', city='%s'",
        id, first_name, last_name, safe_email, safe_city,
        first_name, last_name, safe_email, safe_city);

    if (mysql_query(conn, query)) {
        log_error("MySQL query execution failed: %s", mysql_error(conn));
        return -1;
    }

    log_info("Data stored successfully: ID=%d, Name=%s %s, Email=%s, City=%s", id, first_name, last_name, safe_email, safe_city);
    return 0;
}
