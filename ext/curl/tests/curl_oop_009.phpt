--TEST--
Test CurlHandle::error() & CurlHandle::errno() function with problematic protocol
--CREDITS--
Based on curl_basic_009.phpt by:
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--EXTENSIONS--
curl
--FILE--
<?php

// Make sure the scheme always starts with an alphabetic character.
$url = 'a' . substr(uniqid(),0,6)."://www.example.com";
$ch = (new CurlHandle($url));

try {
  $ch->exec();
} catch (\CurlHandleException $ex) {
  echo 'Caught: ';
  var_dump($ex->getMessage());
}

echo 'Error: ';
var_dump($ch->error());
var_dump($ch->errno());

?>
--EXPECTF--
Caught: string(%d) "%Srotocol%s"
Error: string(%d) "%Srotocol%s"
int(1)
