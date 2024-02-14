--TEST--
Test CurlHandle::error() & CurlHandle::errno() function with problematic proxy
--CREDITS--
Based on curl_basic_010.phpt by:
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--EXTENSIONS--
curl
--SKIPIF--
<?php
    $addr = "www.".uniqid().".".uniqid();
    if (gethostbyname($addr) != $addr) {
        print "skip catch all dns";
    }
?>
--FILE--
<?php

$url = "http://www.example.org";
$ch = (new CurlHandle($url))->setOpt(CURLOPT_PROXY, uniqid().":".uniqid());

try {
  $ch->exec();
} catch (\CurlHandleException $ex) {
  echo 'Caught: ';
  var_dump($ex->getMessage());
}

echo 'Error: ';
var_dump($ch->error());
var_dump($ch->errno());

--EXPECTF--
Caught: string(%d) "%r(Couldn't resolve proxy|Could not resolve proxy:|Could not resolve host:|Could not resolve:|Unsupported proxy syntax in)%r %s"
Error: string(%d) "%r(Couldn't resolve proxy|Could not resolve proxy:|Could not resolve host:|Could not resolve:|Unsupported proxy syntax in)%r %s"
int(5)
