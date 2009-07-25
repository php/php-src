--TEST--
curl_setopt basic tests with CURLOPT_STDERR.
--CREDITS--
Paul Sohier
#phptestfest utrecht
--SKIPIF--
<?php if (!extension_loaded("curl") || false === getenv('PHP_CURL_HTTP_REMOTE_SERVER')) print "skip"; ?>
--FILE--
<?php

$host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

// start testing
echo "*** Testing curl_setopt with CURLOPT_STDERR\n";

$temp_file = tempnam(sys_get_temp_dir(), 'CURL_STDERR');

$handle = fopen($temp_file, 'w');

$url = "{$host}/";
$ch = curl_init();

curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_STDERR, $handle);
$curl_content = curl_exec($ch);

fclose($handle);
unset($handle);
var_dump( file_get_contents($temp_file) );
@unlink($temp_file);

ob_start(); // start output buffering
$handle = fopen($temp_file, 'w');
curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use
curl_setopt($ch, CURLOPT_STDERR, $handle);
$data = curl_exec($ch);
ob_end_clean();

fclose($handle);
unset($handle);
var_dump( file_get_contents($temp_file) );
@unlink($temp_file);

curl_close($ch);

?>
--EXPECTF--
*** Testing curl_setopt with CURLOPT_STDERR
string(%d) "%S"
string(%d) "%S"
