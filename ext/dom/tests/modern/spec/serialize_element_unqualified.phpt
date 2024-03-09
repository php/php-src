--TEST--
Serialize element choose nearest prefix if unqualified name
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root xmlns:p1="u1"><child xmlns:p2="u1"><p1:child2/></child></root>');
echo $dom->saveXML(), "\n";

$dom = DOM\XMLDocument::createFromString('<root xmlns:p1="u1"><child xmlns:p2="u1"></child></root>');
$root = $dom->documentElement;
$child2 = $root->ownerDocument->createElementNS('u1', 'child2');
$root->firstChild->appendChild($child2);
echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:p1="u1"><child xmlns:p2="u1"><p1:child2/></child></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:p1="u1"><child xmlns:p2="u1"><p2:child2/></child></root>
