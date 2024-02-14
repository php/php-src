--TEST--
Test CurlHandle constructor with $url parameter defined
--CREDITS--
Based on curl_basic_015.phpt by:
Jean-Marc Fontaine <jmf@durcommefaire.net>
--EXTENSIONS--
curl
--FILE--
<?php
  $url = 'http://www.example.com/';
  $ch  = new CurlHandle($url);
  var_dump($ch->getInfo(CURLINFO_EFFECTIVE_URL));
?>
--EXPECT--
string(23) "http://www.example.com/"
