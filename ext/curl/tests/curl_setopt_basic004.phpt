--TEST--
curl_setopt() call with CURLOPT_RETURNTRANSFER
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
echo "*** curl_setopt() call with CURLOPT_RETURNTRANSFER set to 1\n";

$url = "{$host}/";
$ch = curl_init();

curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_URL, $url);

$curl_content = curl_exec($ch);
curl_close($ch);

var_dump( $curl_content );

echo "*** curl_setopt() call with CURLOPT_RETURNTRANSFER set to 0\n";

$ch = curl_init();

curl_setopt($ch, CURLOPT_RETURNTRANSFER, 0);
curl_setopt($ch, CURLOPT_URL, $url);
ob_start();
$curl_content = curl_exec($ch);
ob_end_clean();
curl_close($ch);

var_dump( $curl_content );
?>
--EXPECTF--
*** curl_setopt() call with CURLOPT_RETURNTRANSFER set to 1
string(%d) "%a"
*** curl_setopt() call with CURLOPT_RETURNTRANSFER set to 0
bool(true)
