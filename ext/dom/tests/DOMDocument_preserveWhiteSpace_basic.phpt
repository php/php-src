--TEST--
DOMDocument::$preserveWhiteSpace - test ability to read and write property
--CREDITS--
Lev Radin <prokurator@gmail.com>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->load(__DIR__."/book.xml");

var_dump($doc->preserveWhiteSpace);

$doc->preserveWhiteSpace = false;
var_dump($doc->preserveWhiteSpace);

?>
--EXPECT--
bool(true)
bool(false)
