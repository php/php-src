--TEST--
Dom\HTMLDocument::saveHtml() wrong document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$dom->saveHtml(Dom\HTMLDocument::createEmpty());

?>
--EXPECTF--
Fatal error: Uncaught DOMException: Wrong Document Error in %s:%d
Stack trace:
#0 %s(%d): Dom\HTMLDocument->saveHtml(Object(Dom\HTMLDocument))
#1 {main}
  thrown in %s on line %d
