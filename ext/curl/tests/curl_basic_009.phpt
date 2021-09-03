--TEST--
Test curl_error() & curl_errno() function with problematic protocol
--CREDITS--
TestFest 2009 - AFUP - Perrick Penet <perrick@noparking.net>
--EXTENSIONS--
curl
--FILE--
<?php

$url = substr(uniqid(),0,7)."://www.".uniqid().".".uniqid();
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
