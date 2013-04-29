--TEST--
SimpleXML: Attribute creation
--SKIPIF--
<?php 
	if (!extension_loaded('simplexml')) print 'skip';
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

echo $sxe->asXML();

$sxe["attr"] = "new value";

echo $sxe->asXML();

$sxe[] = "error";

__HALT_COMPILER();
?>
===DONE===
--EXPECTF--

Warning: main(): Cannot write or create unnamed attribute in %s012.php on line %d
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="value"/>
<?xml version="1.0" encoding="ISO-8859-1"?>
<foo attr="new value"/>

Fatal error: main(): Cannot create unnamed attribute in %s012.php on line %d
