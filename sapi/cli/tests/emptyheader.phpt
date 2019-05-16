--TEST--
Correctly handle split and empty header
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start("var_dump(getAllheaders());");

$fp = fsockopen(PHP_CLI_SERVER_HOSTNAME, PHP_CLI_SERVER_PORT, $errno, $errmsg, 0.5);

if (!$fp) {
	die("connect failed: " . $errmsg);
}

fwrite($fp, "GET / HTTP/1.1\r\nUser-Agent\r\nAccept: */*\r\nReferer:\r\nHi\r\n\r\n");
fflush($fp);
while (!feof($fp)) {
	echo fgets($fp);
}
fclose($fp);
?>
--EXPECTF--
HTTP/1.1 200 OK
%a
array(3) {
  ["User-AgentAccept"]=>
  string(3) "*/*"
  ["Referer"]=>
  string(0) ""
  ["Hi"]=>
  string(0) ""
}
