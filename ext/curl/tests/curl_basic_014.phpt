--TEST--
Test curl_init() function with basic functionality
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--SKIPIF--
<?php if (!extension_loaded("curl")) exit("skip curl extension not loaded"); ?>
--FILE--
<?php
  $ch = curl_init();
  var_dump($ch);
?>
--EXPECT--
object(CurlHandle)#1 (0) {
}
