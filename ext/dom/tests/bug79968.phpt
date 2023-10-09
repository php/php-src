--TEST--
dom: Bug #79968 - Crash when calling before without valid hierachy
--EXTENSIONS--
dom
--FILE--
<?php

$cdata = new DOMText;

$cdata->before("string");
$cdata->after("string");
$cdata->replaceWith("string");

$dom = new DOMDocument();
$dom->adoptNode($cdata);
var_dump($dom->saveXML($cdata));

?>
--EXPECT--
string(0) ""
