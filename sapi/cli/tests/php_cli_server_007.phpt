--TEST--
Bug #55758 (Digest Authenticate missed in 5.4)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start('header(\'WWW-Authenticate: Digest realm="foo",qop="auth",nonce="XXXXX",opaque="'.md5("foo").'"\');');

$host = PHP_CLI_SERVER_HOSTNAME;
$fp = php_cli_server_connect();

if(fwrite($fp, <<<HEADER
GET / HTTP/1.1
Host: {$host}
Authorization: Basic Zm9vOmJhcg==


HEADER
)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}

?>
--EXPECTF--
HTTP/1.1 401 Unauthorized
Host: %s
Date: %s
Connection: close
X-Powered-By: PHP/%s
WWW-Authenticate: Digest realm="foo",qop="auth",nonce="XXXXX",opaque="acbd18db4cc2f85cedef654fccc4a4d8"
Content-type: text/html; charset=UTF-8
