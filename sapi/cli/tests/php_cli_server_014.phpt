--TEST--
Bug #60477: Segfault after two multipart/form-data POST requestes
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('echo done, "\n";', TRUE);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;
$output = '';

// note: select() on Windows (& some other platforms) has historical issues with
//       timeouts less than 1000 millis(0.5). it may be better to increase these
//       timeouts to 1000 millis(1.0) (fsockopen eventually calls select()).
//       see articles like: http://support.microsoft.com/kb/257821
$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

if(fwrite($fp, <<<HEADER
POST /index.php HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=---------123456789
Content-Length: 70

---------123456789
Content-Type: application/x-www-form-urlencoded
a=b
HEADER
)) {
	while (!feof($fp)) {
		$output .= fgets($fp);
	}
}

fclose($fp);

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if(fwrite($fp, <<<HEADER
POST /main/no-exists.php HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=---------123456789
Content-Length: 70

---------123456789
Content-Type: application/x-www-form-urlencoded
a=b
HEADER
)) {
	while (!feof($fp)) {
		$output .= fgets($fp);
	}
}

echo preg_replace("/<style>(.*?)<\/style>/s", "<style>AAA</style>", $output), "\n";
fclose($fp);

?>
--EXPECTF--

HTTP/1.1 200 OK
Host: %s
Connection: close
X-Powered-By: %s
Content-type: %s

done
HTTP/1.1 404 Not Found
Host: %s
Connection: close
Content-Type: %s
Content-Length: %d

<!doctype html><html><head><title>404 Not Found</title><style>AAA</style>
</head><body><h1>Not Found</h1><p>The requested resource /main/no-exists.php was not found on this server.</p></body></html>
