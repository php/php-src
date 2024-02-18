--TEST--
Test CurlHandle::error() & CurlHandle::errno() function without url
--CREDITS--
Based on curl_basic_007.phpt by:
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--EXTENSIONS--
curl
--FILE--
<?php

//In January 2008 , level 7.18.0 of the curl lib, many of the messages changed.
//The final crlf was removed. This test is coded to work with or without the crlf.

$ch = new CurlHandle;
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
Caught: string(%d) "No URL set%A"
Error: string(%d) "No URL set%A"
int(3)
