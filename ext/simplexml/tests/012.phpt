--TEST--
SimpleXML and Attribute creation
--SKIPIF--
<?php 
	if (!extension_loaded('simplexml')) print 'skip';
	if (!class_exists('RecursiveIteratorIterator')) print 'skip RecursiveIteratorIterator not available';
?>
--FILE--
<?php 

$xml =<<<EOF
<?xml version="1.0" encoding="ISO-8859-1" ?>
<foo/>
EOF;

$sxe = simplexml_load_string($xml);


$sxe[""] = "warning";
$sxe["attr"] = "value";

echo $sxe->to_xml_string();

$sxe["attr"] = "new value";

echo $sxe->to_xml_string();

$sxe[] = "error";

?>
===DONE===
--EXPECTF--

Warning: main(): Cannot write or create unnamed attribute in %s012.php on line %d
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="value"/>
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="new value"/>

Fatal error: main(): Cannot create unnamed attribute in %s012.php on line %d
