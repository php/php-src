--TEST--
Test that cloning of Curl objects is supported
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();

$ch1 = curl_init();
curl_setopt($ch1, CURLOPT_URL, $host);
curl_setopt($ch1, CURLOPT_RETURNTRANSFER, 1);
curl_exec($ch1);

$ch2 = clone $ch1;
curl_setopt($ch2, CURLOPT_RETURNTRANSFER, 0);

var_dump(curl_getinfo($ch1, CURLINFO_EFFECTIVE_URL) === curl_getinfo($ch2, CURLINFO_EFFECTIVE_URL));
curl_exec($ch2);

?>
--EXPECT--
bool(true)
Hello World!
Hello World!
