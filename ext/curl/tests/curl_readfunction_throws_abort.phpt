--TEST--
CURLOPT_READFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_READFUNCTION')) {
    die('skip CURLOPT_READFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

$file = new CURLFile(__DIR__ . '/curl_testdata1.txt');
curl_setopt($ch, CURLOPT_POST, 1);

echo "Test: read function throws exception\n";
curl_setopt($ch, CURLOPT_READFUNCTION,
    function (): int {
        throw new Exception('read exception');
    }
);

try {
    curl_exec($ch);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump(curl_errno($ch) === CURLE_ABORTED_BY_CALLBACK);

echo "Test: read function is null\n";
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_READFUNCTION, null);
curl_exec($ch);
var_dump(curl_errno($ch) === CURLE_OK);

?>
--EXPECTF--
Test: read function throws exception
Exception: read exception
bool(true)
Test: read function is null
bool(true)
