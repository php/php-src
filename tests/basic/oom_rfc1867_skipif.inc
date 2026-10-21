<?php
if (!getenv('TEST_PHP_CGI_EXECUTABLE')) {
    die("skip php-cgi not available");
}
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die("skip not for Windows in CI - probably resource issue");
}
