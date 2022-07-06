--TEST--
Bug #61335 - Access to array node returns wrong truth value
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) die("skip SimpleXML not available");
?>
--FILE--
<?php
$rec1 = simplexml_load_string("<foo><bar>aa</bar>\n</foo>");
$rec2 = simplexml_load_string("<foo><bar>aa</bar></foo>");

if ($rec1->bar[0])      echo "NONEMPTY1\n";
if ($rec1->bar[0] . "") echo "NONEMPTY2\n";
if ($rec2->bar[0])      echo "NONEMPTY3\n";
?>
--EXPECT--
NONEMPTY1
NONEMPTY2
NONEMPTY3
