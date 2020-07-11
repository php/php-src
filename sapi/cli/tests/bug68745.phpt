--TEST--
Bug #68745 (Invalid HTTP requests make web server segfault)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump(count($_SERVER));', 'not-index.php');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, "GET www.example.com:80 HTTP/1.1\r\n\r\n")) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

int(%d)
