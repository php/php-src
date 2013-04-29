--TEST--
Bug #60591 (Memory leak when access a non-exists file)
--DESCRIPTION--
this is an indirect test for bug 60591, since mem leak is reproted in the server side
and require php compiled with --enable-debug
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
if (preg_match('/\.(?:png|jpg|jpeg|gif)$/', $_SERVER["REQUEST_URI"]))
        return false; // serve the requested resource as-is.
else {
        echo "here";
}
PHP
);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /no-exists.jpg HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
        break;
	}
}

fclose($fp);
?>
--EXPECTF--
HTTP/1.1 404 Not Found
