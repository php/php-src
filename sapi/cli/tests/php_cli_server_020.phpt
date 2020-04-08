--TEST--
Use SEARCH as a HTTP verb
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["REQUEST_METHOD"]);');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
SEARCH / HTTP/1.1
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

string(6) "SEARCH"
