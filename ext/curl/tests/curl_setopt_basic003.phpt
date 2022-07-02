--TEST--
curl_setopt() call with CURLOPT_HTTPHEADER
--CREDITS--
Paul Sohier
#phptestfest utrecht
--EXTENSIONS--
curl
--FILE--
<?php

include 'server.inc';
$host = curl_cli_server_start();

// start testing
echo "*** curl_setopt() call with CURLOPT_HTTPHEADER\n";

$url = "{$host}/";
$ch = curl_init();

try {
    curl_setopt($ch, CURLOPT_HTTPHEADER, 1);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

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
--EXPECT--
*** curl_setopt() call with CURLOPT_HTTPHEADER
curl_setopt(): The CURLOPT_HTTPHEADER option must have an array value
bool(false)
bool(true)
