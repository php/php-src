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

curl_setopt($ch, CURLOPT_HEADERFUNCTION,
    function (): int {
        throw new Exception('header exception');
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
header exception
bool(true)
