--TEST--
Test curl_getinfo() function with CURLINFO_CONTENT_TYPE parameter
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
  include 'server.inc';
  $host = curl_cli_server_start();
  $url  = "{$host}/get.php?test=contenttype";

  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, $url);
  curl_exec($ch);
  var_dump(curl_getinfo($ch, CURLINFO_CONTENT_TYPE));
  curl_close($ch);
?>
===DONE===
--EXPECTF--
string(24) "text/plain;charset=utf-8"
===DONE===
