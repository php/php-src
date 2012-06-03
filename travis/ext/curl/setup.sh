#!/bin/bash
export PHP_CURL_HTTP_REMOTE_SERVER="http://localhost"
cd ./ext/curl/tests/responder
sudo php -S localhost:80 &
cd -