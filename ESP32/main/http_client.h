#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "esp_err.h"

void http_get_request();
void http_post_request(const char *data);

#endif // HTTP_CLIENT_H
