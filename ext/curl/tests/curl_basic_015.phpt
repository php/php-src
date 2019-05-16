--TEST--
Test curl_init() function with $url parameter defined
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php if (!extension_loaded("curl")) exit("skip curl extension not loaded"); ?>
--FILE--
<?php
  $url = 'http://www.example.com/';
  $ch  = curl_init($url);
  var_dump($url == curl_getinfo($ch, CURLINFO_EFFECTIVE_URL));
?>
===DONE===
--EXPECT--
bool(true)
===DONE===
