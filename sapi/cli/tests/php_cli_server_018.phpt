--TEST--
Fixed default index files matching
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_SERVER["SCRIPT_FILENAME"]);', TRUE);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /not/such/path HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
		break;
	}
}

fclose($fp);

$exits_dir = __DIR__ . "/real_dir";
mkdir($exits_dir);

file_put_contents($exits_dir . "/index.php", '<?php var_dump($_SERVER["PHP_SELF"], $_SERVER["SCRIPT_NAME"], $_SERVER["PATH_INFO"]); ?>');

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /real_dir HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}

fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /real_dir/not-exits HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
		break;
	}
}

fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /real_dir/index.php/path/info HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}

fclose($fp);

@unlink($exits_dir . "/index.php");
@rmdir($exits_dir);
?>
--EXPECTF--
HTTP/1.1 404 Not Found
HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html

string(19) "/real_dir/index.php"
string(19) "/real_dir/index.php"
NULL
HTTP/1.1 404 Not Found
HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html

string(29) "/real_dir/index.php/path/info"
string(19) "/real_dir/index.php"
string(10) "/path/info"
