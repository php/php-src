--TEST--
SERVER_PROTOCOL header availability
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["SERVER_PROTOCOL"]);');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);

$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.0
Host: {$host}


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(8) "HTTP/1.1"
HTTP/1.0 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(8) "HTTP/1.0"
