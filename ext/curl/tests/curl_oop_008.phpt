--TEST--
Test CurlHandle::error() & CurlHandle::errno() function with problematic host
--CREDITS--
Based on curl_basic_008.phpt by:
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

$url = "http://www.".uniqid().".".uniqid();
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
Caught: %s resolve%s
Error: %s resolve%s
int(6)
