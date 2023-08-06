--TEST--
Manually call __construct() - processing instruction variation
--EXTENSIONS--
dom
--FILE--
<?php

$pi = new DOMProcessingInstruction('name1', 'value1');
var_dump($pi->target, $pi->data);
$pi->__construct('name2', 'value2');
var_dump($pi->target, $pi->data);

$doc = new DOMDocument();
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container/>
XML);
$doc->documentElement->appendChild($pi);
echo $doc->saveXML();

$pi->__construct('name3', 'value3');
$doc->documentElement->appendChild($pi);
echo $doc->saveXML();

?>
--EXPECTF--
string(5) "name1"
string(6) "value1"

Fatal error: Uncaught Error: Couldn't fetch DOMProcessingInstruction. Node no longer exists in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
