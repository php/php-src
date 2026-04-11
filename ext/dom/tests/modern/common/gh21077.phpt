--TEST--
GH-21077 (Accessing Dom\Node::baseURI can throw TypeError)
--EXTENSIONS--
dom
--CREDITS--
mbeccati
--FILE--
<?php

$implementation = new Dom\Implementation();
$node = $implementation->createDocumentType('html', 'publicId', 'systemId');

var_dump($node->baseURI);

$dom = Dom\XMLDocument::createEmpty();
$dom->append($node = $dom->importNode($node));

var_dump($dom->saveXML());

var_dump($node->baseURI);

?>
--EXPECT--
string(11) "about:blank"
string(84) "<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "publicId" "systemId">
"
string(11) "about:blank"
