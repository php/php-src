--TEST--
Returning refcounted value from callback must not leak
--EXTENSIONS--
curl
zend_test
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();

$url = "{$host}/get.inc";
$ch = curl_init($url);

function return_non_interned_string() {
    return zend_return_non_interned_string('x');
}

curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_PROGRESSFUNCTION, 'return_non_interned_string');
curl_setopt($ch, CURLOPT_XFERINFOFUNCTION, 'return_non_interned_string');
curl_setopt($ch, CURLOPT_WRITEFUNCTION, 'return_non_interned_string');
curl_setopt($ch, CURLOPT_HEADERFUNCTION, 'return_non_interned_string');
echo curl_exec($ch), PHP_EOL;
echo "ok";
?>
--EXPECT--
ok
