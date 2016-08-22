--TEST--
Bug #54973 SimpleXML casts intergers wrong.
--SKIPIF--
<?php 
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platforms only");
--FILE--
<?php

$xml = simplexml_load_string("<xml><number>214748364800</number></xml>");
echo $xml->number . "\n"; // the proper number

$int = $xml->number / 1024 / 1024 / 1024; // initial cast to an int causes problems
echo $int . "\n";

$strint = strval($xml->number) / 1024 / 1024 / 1024; // external cast to int behavior
echo $strint . "\n";

$double = (double) $xml->number / 1024 / 1024 / 1024; // hard cast to a double fixes it
echo $double . "\n";
--EXPECT--
214748364800
200
200
200
