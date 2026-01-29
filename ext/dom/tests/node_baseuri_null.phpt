--TEST--
DOMNode::baseURI can be NULL
--EXTENSIONS--
dom
--FILE--
<?php

$implementation = new \Dom\Implementation();
$node = $implementation->createDocumentType('html', 'publicId', 'systemId');

var_dump($node->baseURI);

--EXPECT--
NULL
