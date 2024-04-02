--TEST--
DOM\HTMLDocument::saveHTMLFile() empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));
$dom->saveHTMLFile("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: DOM\HTMLDocument::saveHTMLFile(): Argument #1 ($filename) must not be empty in %s:%d
Stack trace:
#0 %s(%d): DOM\HTMLDocument->saveHTMLFile('')
#1 {main}
  thrown in %s on line %d
