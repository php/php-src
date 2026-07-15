--TEST--
CURLOPT_PROGRESSFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_PROGRESSFUNCTION')) {
    die('skip CURLOPT_PROGRESSFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_PROGRESSFUNCTION,
    function (): int {
        throw new Exception('info exception');
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
info exception
bool(true)
