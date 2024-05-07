--TEST--
Dom\HTMLDocument::saveHTMLFile() empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));
$dom->saveHTMLFile("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Dom\HTMLDocument::saveHTMLFile(): Argument #1 ($filename) must not be empty in %s:%d
Stack trace:
#0 %s(%d): Dom\HTMLDocument->saveHTMLFile('')
#1 {main}
  thrown in %s on line %d
