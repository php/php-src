#!/bin/bash
export PHP_CURL_HTTP_REMOTE_SERVER="http://localhost:8080"
cd ./ext/curl/tests/responder
php -S localhost:8080 &
cd -
