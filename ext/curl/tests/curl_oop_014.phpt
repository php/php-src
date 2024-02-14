--TEST--
Test CurlHandle object with basic functionality
--CREDITS--
Based on curl_basic_014.phpt
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  $ch = new CurlHandle;
  var_dump($ch);
?>
--EXPECT--
object(CurlHandle)#1 (0) {
}
