--TEST--
Post a file
--SKIPIF--
<?php
include "skipif.inc"; 
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('var_dump($_FILES);');

list($host, $port) = explode(':', PHP_CLI_SERVER_ADDRESS);
$port = intval($port)?:80;

$fp = fsockopen($host, $port, $errno, $errstr, 0.5);
if (!$fp) {
  die("connect failed");
}

$post_data = <<<POST
-----------------------------114782935826962
Content-Disposition: form-data; name="userfile"; filename="laruence.txt"
Content-Type: text/plain

I am not sure about this.

-----------------------------114782935826962--


POST;

$post_len = strlen($post_data);

if(fwrite($fp, <<<HEADER
POST / HTTP/1.1
Host: {$host}
Content-Type: multipart/form-data; boundary=---------------------------114782935826962
Content-Length: {$post_len}


{$post_data}
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
Connection: close
X-Powered-By: PHP/%s
Content-type: text/html; charset=UTF-8

array(1) {
  ["userfile"]=>
  array(5) {
    ["name"]=>
    string(12) "laruence.txt"
    ["type"]=>
    string(10) "text/plain"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(26)
  }
}
