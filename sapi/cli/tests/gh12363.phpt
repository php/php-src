--TEST--
Ensure a single Date header is present
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
header('Date: Mon, 25 Mar 1985 00:20:00 GMT');
PHP
);

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
?>
--EXPECTF--
HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: %s
Date: Mon, 25 Mar 1985 00:20:00 GMT
Content-type: text/html; charset=UTF-8

