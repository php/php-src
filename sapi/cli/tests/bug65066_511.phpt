--TEST--
Bug #65066 (Cli server not responsive when responding with 422 http status code): 511 status code
--INI--
allow_url_fopen=1
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('http_response_code(511);');

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
?>
--EXPECTF--
HTTP/1.1 511 Network Authentication Required
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8
