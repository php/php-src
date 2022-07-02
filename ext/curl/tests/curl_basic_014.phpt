--TEST--
Test curl_init() function with basic functionality
--CREDITS--
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  $ch = curl_init();
  var_dump($ch);
?>
--EXPECT--
object(CurlHandle)#1 (0) {
}
