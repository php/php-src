--TEST--
Test curl_error() & curl_errno() function with problematic host
--CREDITS--
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--SKIPIF--
<?php
	if (!extension_loaded("curl")) print "skip"; 
	$addr = "www.".uniqid().".".uniqid();
	if (gethostbyname($addr) != $addr) {
		print "skip catch all dns";
	}
?>
--FILE--
<?php

$url = "http://www.".uniqid().".".uniqid();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);

curl_exec($ch);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));
curl_close($ch);


?>
--EXPECTF--
%s resolve%s
int(6)
