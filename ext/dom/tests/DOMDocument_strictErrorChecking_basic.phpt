--TEST--
DomDocument::$strictErrorChecking - basic test
--CREDITS--
Vincent Tsao <notes4vincent@gmail.com>
# TestFest 2009 NYPHP
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->load(__DIR__."/book.xml");

var_dump($doc->strictErrorChecking);

$doc->strictErrorChecking = false;
var_dump($doc->strictErrorChecking);

?>
--EXPECT--
bool(true)
bool(false)
