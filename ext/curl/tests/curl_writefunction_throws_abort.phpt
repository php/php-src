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

curl_setopt($ch, CURLOPT_WRITEFUNCTION,
    function (): int {
        throw new Exception('write exception');
    }
);

try {
    curl_exec($ch);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

var_dump(curl_errno($ch) === CURLE_WRITE_ERROR);

?>
--EXPECTF--
write exception
bool(true)
