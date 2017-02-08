--TEST--
bool checkdnsrr ( string $host [, string $type = "MX" ] );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - @phpsp - sao paulo - br 
--SKIPIF--
<?php
if (getenv("SKIP_ONLINE_TESTS")) { die('skip: online test'); }
?>
--FILE--
<?php
$serverUrl = "bugs.php.net";
var_dump(checkdnsrr());
var_dump(checkdnsrr($serverUrl));
var_dump(checkdnsrr($serverUrl, "A"));
var_dump(checkdnsrr($serverUrl, "MX"));
var_dump(checkdnsrr($serverUrl, "NS"));
var_dump(checkdnsrr($serverUrl, "SOA"));
var_dump(checkdnsrr($serverUrl, "PTR"));
var_dump(checkdnsrr($serverUrl, "CNAME"));
var_dump(checkdnsrr($serverUrl, "AAAA"));
var_dump(checkdnsrr($serverUrl, "A6"));
var_dump(checkdnsrr($serverUrl, "SRV"));
var_dump(checkdnsrr($serverUrl, "NAPTR"));
var_dump(checkdnsrr($serverUrl, "TXT"));
var_dump(checkdnsrr($serverUrl, "ANY"));
?>
--EXPECTF--
Warning: checkdnsrr() expects at least %d parameter, %i given in %s on line %d
NULL
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
