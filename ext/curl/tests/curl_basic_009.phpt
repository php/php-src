--TEST--
Test curl_error() & curl_errno() function with problematic protocol
--CREDITS--
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--FILE--
<?php

$url = uniqid()."://www.".uniqid().".".uniqid();
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);

curl_exec($ch);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));
curl_close($ch);


?>
--EXPECTREGEX--
string\(\d+\) "([^\r\n]*rotocol[^\r\n]+|Could not resolve host: .+)"
int\(\d\)
