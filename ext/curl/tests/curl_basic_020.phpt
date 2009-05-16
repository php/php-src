--TEST--
Test curl_getinfo() function with CURLINFO_HTTP_CODE parameter
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php
if (!extension_loaded("curl")) exit("skip curl extension not loaded");
if (false === getenv('PHP_CURL_HTTP_REMOTE_SERVER'))  exit("skip PHP_CURL_HTTP_REMOTE_SERVER env variable is not defined");
?>
--FILE--
<?php
  $host = getenv('PHP_CURL_HTTP_REMOTE_SERVER');

  $url = "{$host}/get.php?test=";
  $ch  = curl_init();
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_exec($ch);
  var_dump(curl_getinfo($ch, CURLINFO_HTTP_CODE));
  curl_close($ch);
?>
===DONE===
--EXPECTF--
Hello World!
Hello World!int(200)
===DONE===
