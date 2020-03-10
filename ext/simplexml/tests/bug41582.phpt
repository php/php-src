--TEST--
Bug #41582 (SimpleXML crashes when accessing newly created element)
--EXTENSIONS--
simplexml
--FILE--
<?php

$xml = new SimpleXMLElement('<?xml version="1.0" standalone="yes"?>
<collection></collection>');

$xml->movie[]->characters->character[0]->name = 'Miss Coder';
$movie = $xml->movie[0];
$movie->characters->character[1]->name = 'Mr Bug';

echo($xml->asXml());

?>
--EXPECT--
<?xml version="1.0" standalone="yes"?>
<collection><movie><characters><character><name>Miss Coder</name></character><character><name>Mr Bug</name></character></characters></movie></collection>
