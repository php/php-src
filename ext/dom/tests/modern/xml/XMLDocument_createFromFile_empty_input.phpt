--TEST--
XMLDocument::createFromFile() - empty input
--EXTENSIONS--
dom
--FILE--
<?php
$dom = Dom\XMLDocument::createFromFile("php://memory");
var_dump($dom->documentURI);
fclose($memory);
?>
--EXPECTF--
Warning: Dom\XMLDocument::createFromFile(): Document is empty in php://memory, line: 1 in %s on line %d

Fatal error: Uncaught DOMException: XML fragment is not well-formed in %s:%d
Stack trace:
#0 %s(%d): Dom\XMLDocument::createFromFile('php://memory')
#1 {main}
  thrown in %s on line %d
