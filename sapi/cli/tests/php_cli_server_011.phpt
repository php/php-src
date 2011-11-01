--TEST--
Bug #60180 ($_SERVER["PHP_SELF"] incorrect)
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('sytanx error;', TRUE);

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

$logo_id = php_logo_guid();

if(fwrite($fp, <<<HEADER
GET /?={$logo_id} HTTP/1.1
Host: {$host}


HEADER
)) {
	while (!feof($fp)) {
		if (("Content-Type: image/gif") ==  trim(fgets($fp))) {
			echo "okey";
			break;
		}
	}
}

fclose($fp);

?>
--EXPECTF--
okey
