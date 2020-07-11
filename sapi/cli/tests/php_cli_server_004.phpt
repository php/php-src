--TEST--
Bug #55747 (request headers missed in $_SERVER)
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('foreach($_SERVER as $k=>$v) { if (!strncmp($k, "HTTP", 4)) var_dump( $k . ":" . $v); }');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host:{$host}
User-Agent:dummy
Custom:foo
Referer:http://www.php.net/


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

string(19) "HTTP_HOST:localhost"
string(21) "HTTP_USER_AGENT:dummy"
string(15) "HTTP_CUSTOM:foo"
string(32) "HTTP_REFERER:http://www.php.net/"
