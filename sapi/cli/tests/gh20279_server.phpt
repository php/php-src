--TEST--
GH-18582: Allow http_response_code() to clear HTTP start-line
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
var_dump(ini_get("register_argc_argv"));
PHP);
$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();
if (fwrite($fp, "GET / HTTP/1.1\nHost: {$host}\n\n")) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}
fclose($fp);
--EXPECTF--
HTTP/1.1 200 OK
Host: localhost
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(1) "1"
