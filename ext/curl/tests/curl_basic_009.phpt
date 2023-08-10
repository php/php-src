--TEST--
Test curl_error() & curl_errno() function with problematic protocol
--CREDITS--
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--EXTENSIONS--
curl
--FILE--
<?php

// Make sure the scheme always starts with an alphabetic character.
$url = 'a' . substr(uniqid(),0,6)."://www.example.com";
$ch = curl_init();
curl_setopt($ch, CURLOPT_URL, $url);

curl_exec($ch);
var_dump(curl_error($ch));
var_dump(curl_errno($ch));
curl_close($ch);


?>
--EXPECTF--
string(%d) "%Srotocol%s"
int(1)
