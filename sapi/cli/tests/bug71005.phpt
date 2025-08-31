--TEST--
Bug #71005 (Segfault in php_cli_server_dispatch_router())
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$code = <<<'EOF'
set_exception_handler(function () { echo 'goodbye'; });
throw new Exception;
EOF;

include "php_cli_server.inc";
php_cli_server_start($code);

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
HTTP/1.1 200 OK
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

goodbye
