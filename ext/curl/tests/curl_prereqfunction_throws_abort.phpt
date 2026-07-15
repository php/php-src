--TEST--
CURLOPT_PREREQFUNCTION aborts transfer when callback throws
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_PREREQFUNCTION')) {
    die('skip CURLOPT_PREREQFUNCTION not available');
}
?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();
$ch = curl_init("{$host}/get.inc");

curl_setopt($ch, CURLOPT_PREREQFUNCTION,
    function (): int {
        throw new Exception('prereq exception');
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
prereq exception
bool(true)
