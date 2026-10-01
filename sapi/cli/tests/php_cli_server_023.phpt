--TEST--
Built-in web server reflects arbitrary `Host` headers
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
include "php_cli_server.inc";
php_cli_server_start(<<<'PHP'
echo "Request headers:\n";
var_dump(getallheaders());
PHP
);

$req = <<<END
GET / HTTP/1.1
Host: Foo Bar Baz



END;

$fp = php_cli_server_connect();
if (fwrite($fp, $req)) {
	while (!feof($fp)) {
		echo fgets($fp);
	}
}
fclose($fp);

?>
--EXPECTF--
HTTP/1.1 200 OK
Date: %s
Connection: close
X-Powered-By: %s
Content-type: text/html; charset=UTF-8

Request headers:
array(1) {
  ["Host"]=>
  string(11) "Foo Bar Baz"
}
