--TEST--
XMLDocument::createFromFile() - empty input
--EXTENSIONS--
dom
--FILE--
<?php
$dom = DOM\XMLDocument::createFromFile("php://memory");
var_dump($dom->documentURI);
fclose($memory);
?>
--EXPECTF--
Warning: DOM\XMLDocument::createFromFile(): Document is empty in php://memory, line: 1 in %s on line %d

Fatal error: Uncaught Exception: XML document is malformed in %s:%d
Stack trace:
#0 %s(%d): DOM\XMLDocument::createFromFile('php://memory')
#1 {main}
  thrown in %s on line %d
