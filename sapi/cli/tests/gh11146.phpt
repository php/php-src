--TEST--
GH-11146: The built-in webserver will add ";charset=UTF-8" to the end of the Content-Type response header of the SSE response.
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(
    <<<'CODE'
header('Content-Type: text/event-stream');
for ($i = 0; $i < 3; ++$i) {
    echo 'data: ' . $i . "\n\n";
}
CODE
);

$host = PHP_CLI_SERVER_HOSTNAME;

$fp = php_cli_server_connect();
if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}


HEADER)) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}
?>
--EXPECT--
HTTP/1.1 200 OK
Host: localhost
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/event-stream

data: 0

data: 1

data: 2
