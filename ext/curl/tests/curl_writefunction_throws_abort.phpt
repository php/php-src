--TEST--
CURLOPT_WRITEFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_WRITEFUNCTION')) {
    die('skip CURLOPT_WRITEFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

echo "Test: write function throws exception\n";
curl_setopt($ch, CURLOPT_WRITEFUNCTION,
    function (): int {
        throw new Exception('write exception');
    }
);

try {
    curl_exec($ch);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(curl_errno($ch) === CURLE_WRITE_ERROR);

echo "Test: write function is null\n";
curl_setopt($ch, CURLOPT_WRITEFUNCTION, null);
curl_exec($ch);
var_dump(curl_errno($ch) === CURLE_OK);

?>
--EXPECTF--
Test: write function throws exception
Exception: write exception
bool(true)
Test: write function is null
Hello World!
Hello World!bool(true)
