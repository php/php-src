--TEST--
Implement Req #60850 (Built in web server does not set $_SERVER['SCRIPT_FILENAME'] when using router)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
var_dump($_SERVER['SCRIPT_FILENAME']);
PHP
);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
POST / HTTP/1.1
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
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

string(%d) "%sindex.php"
