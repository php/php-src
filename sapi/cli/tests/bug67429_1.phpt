--TEST--
FR #67429 (CLI server is missing some new HTTP response codes)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

php_cli_server_start(<<<PHP
http_response_code(308);
PHP
);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1


HEADER
)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 308 Permanent Redirect
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8
