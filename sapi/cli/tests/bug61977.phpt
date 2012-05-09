--TEST--
Bug #61977 (Need CLI web-server support for files with .htm & svg extensions)
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('<?php ?>', true);
$doc_root = __DIR__;

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

file_put_contents($doc_root . '/foo.html', '');
if(fwrite($fp, <<<HEADER
GET /foo.html HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.html => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.html');
fclose($fp);


$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
file_put_contents($doc_root . '/foo.htm', '');
if(fwrite($fp, <<<HEADER
GET /foo.htm HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.htm => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.htm');
fclose($fp);


$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
file_put_contents($doc_root . '/foo.svg', '');
if(fwrite($fp, <<<HEADER
GET /foo.svg HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.svg => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.svg');
fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
file_put_contents($doc_root . '/foo.css', '');
if(fwrite($fp, <<<HEADER
GET /foo.css HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.css => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.css');
fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
file_put_contents($doc_root . '/foo.js', '');
if(fwrite($fp, <<<HEADER
GET /foo.js HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.js => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.js');
fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}
file_put_contents($doc_root . '/foo.png', '');
if(fwrite($fp, <<<HEADER
GET /foo.png HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		$text = fgets($fp);
        if (strncasecmp("Content-type:", $text, 13) == 0) {
           echo "foo.png => ", $text;
        }
	}
}
@unlink($doc_root . '/foo.png');
fclose($fp);
?>
--EXPECTF--
foo.html => Content-Type: text/html; charset=UTF-8
foo.htm => Content-Type: text/html; charset=UTF-8
foo.svg => Content-Type: image/svg+xml
foo.css => Content-Type: text/css; charset=UTF-8
foo.js => Content-Type: text/javascript; charset=UTF-8
foo.png => Content-Type: image/png
