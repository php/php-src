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
curl_setopt($ch, CURLOPT_POST, ['file' => $file]);

curl_setopt($ch, CURLOPT_READFUNCTION,
    function (): int {
        throw new Exception('read exception');
    }
);

try {
    curl_exec($ch);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

var_dump(curl_errno($ch) === CURLE_ABORTED_BY_CALLBACK);

?>
--EXPECTF--
read exception
bool(true)
