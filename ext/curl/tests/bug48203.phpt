--TEST--
Bug #48203 (Crash when CURLOPT_STDERR is set to regular file)
--SKIPIF--
<?php 
if (!extension_loaded("curl")) {
	exit("skip curl extension not loaded");
}
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  {
	exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
}
?>
--FILE--
<?php

$fp = fopen(dirname(__FILE__) . '/bug48203.tmp', 'w');

$ch = curl_init();

curl_setopt($ch, CURLOPT_VERBOSE, 1);
curl_setopt($ch, CURLOPT_STDERR, $fp);
curl_setopt($ch, CURLOPT_URL, "");

fclose($fp); // <-- premature close of $fp caused a crash!

curl_exec($ch);

echo "Ok\n";

?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug48203.tmp'); ?>
--EXPECT--
Ok
