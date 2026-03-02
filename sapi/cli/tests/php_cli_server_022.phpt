--TEST--
Check PHP file body is executed
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
header('Bar-Foo: Foo');
echo "Hello world\n";
var_dump(true);
PHP
);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}
Foo-Bar: Bar


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
Bar-Foo: Foo
Content-type: text/html; charset=UTF-8

Hello world
bool(true)
