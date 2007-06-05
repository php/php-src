--TEST--
Bug #41582 (SimpleXML crashes when accessing newly created element)
--FILE--
<?php

$xml = new SimpleXMLElement(b'<?xml version="1.0" standalone="yes"?><collection></collection>');

$xml->movie[]->characters->character[0]->name = b'Miss Coder';

var_dump($xml->asXml());

echo "Done\n";
?>
--EXPECTF--	
Fatal error: Cannot use object of type stdClass as array in %s on line %d
