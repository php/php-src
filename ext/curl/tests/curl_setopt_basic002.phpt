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

$temp_file = tempnam(sys_get_temp_dir(), '');
$handle = fopen($temp_file, 'w');

$url = "{$host}/";
$ch = curl_init();

curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_STDERR, $handle);

$curl_content = curl_exec($ch);
fclose($handle);

var_dump( curl_error($ch) );
var_dump( file_get_contents($temp_file));

@unlink ($temp_file);
$handle = fopen($temp_file, 'w');

ob_start(); // start output buffering
curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_STDERR, $handle);
curl_setopt($ch, CURLOPT_URL, $url); //set the url we want to use

$data = curl_exec($ch);
fclose($handle);
ob_end_clean();
var_dump(file_get_contents($temp_file));

curl_close($ch);
@unlink($temp_file);
?>
--EXPECTF--
*** Testing curl_setopt with CURLOPT_STDERR
string(%d) "%s"
string(%d) "%S"
string(%d) "%S"
