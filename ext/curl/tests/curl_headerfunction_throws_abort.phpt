--TEST--
CURLOPT_HEADERFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_HEADERFUNCTION')) {
    die('skip CURLOPT_HEADERFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

echo "Test: header function throws exception\n";
curl_setopt($ch, CURLOPT_HEADERFUNCTION,
    function (): int {
        throw new Exception('header exception');
    }
);

try {
    curl_exec($ch);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(curl_errno($ch) === CURLE_WRITE_ERROR);

echo "Test: header function is null\n";
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_HEADERFUNCTION, null);
curl_exec($ch);
var_dump(curl_errno($ch) === CURLE_OK);

?>
--EXPECTF--
Test: header function throws exception
Exception: header exception
bool(true)
Test: header function is null
bool(true)
