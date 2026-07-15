--TEST--
CURLOPT_XFERINFOFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_XFERINFOFUNCTION')) {
    die('skip CURLOPT_XFERINFOFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_XFERINFOFUNCTION,
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
