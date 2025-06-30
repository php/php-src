--TEST--
GH-18582: Allow http_response_code() to clear HTTP start-line
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";

php_cli_server_start(<<<'PHP'
http_response_code(401);
header('HTTP/1.1 404 Not Found');
$is_404 = http_response_code(403);
$should_be_404_but_is_403 = http_response_code();
echo $is_404 . PHP_EOL;
echo $should_be_404_but_is_403 . PHP_EOL;
PHP);

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();
if (fwrite($fp, "GET / HTTP/1.1\nHost: {$host}\n\n")) {
    while (!feof($fp)) {
        echo fgets($fp);
    }
}
fclose($fp);
?>
--EXPECTF--
HTTP/1.1 403 Forbidden
Host: %s
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

404
403
