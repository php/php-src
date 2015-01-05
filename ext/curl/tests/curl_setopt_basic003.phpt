--TEST--
curl_setopt() call with CURLOPT_HTTPHEADER
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();

// start testing
echo "*** curl_setopt() call with CURLOPT_HTTPHEADER\n";

$url = "{$host}/";
$ch = curl_init();

curl_setopt($ch, CURLOPT_HTTPHEADER, 1);

$curl_content = curl_exec($ch);
curl_close($ch);

var_dump( $curl_content );

$ch = curl_init();

ob_start(); // start output buffering
curl_setopt($ch, CURLOPT_HTTPHEADER, array());
curl_setopt($ch, CURLOPT_URL, $host);

$curl_content = curl_exec($ch);
ob_end_clean();
curl_close($ch);

var_dump( $curl_content );
?>
--EXPECTF--
*** curl_setopt() call with CURLOPT_HTTPHEADER

Warning: curl_setopt(): You must pass either an object or an array with the CURLOPT_HTTPHEADER argument in %s on line %d
bool(false)
bool(true)
