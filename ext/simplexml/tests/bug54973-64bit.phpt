--TEST--
Bug #54973 SimpleXML casts intergers wrong.
--SKIPIF--
<?php 
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platforms only");
--FILE--
<?php

$xml = simplexml_load_string("<xml><number>922337203685477580800</number></xml>");
echo $xml->number . "\n"; // the proper number

// initial cast to an int causes problems
$int = $xml->number / 1024 / 1024 / 1024 / 1024 / 1024 / 1024;
echo $int . "\n";

// external cast to int behavior
$strint = strval($xml->number) / 1024 / 1024 / 1024 / 1024 / 1024 / 1024;
echo $strint . "\n";

// hard cast to a double fixes it
$double = (double) $xml->number / 1024 / 1024 / 1024 / 1024 / 1024 / 1024;
echo $double . "\n";
--EXPECT--
922337203685477580800
800
800
800
