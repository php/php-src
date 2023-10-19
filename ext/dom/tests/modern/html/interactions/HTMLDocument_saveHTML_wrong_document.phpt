--TEST--
DOM\HTMLDocument::saveHTML() wrong document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$dom->saveHTML(DOM\HTMLDocument::createEmpty());

?>
--EXPECTF--
Fatal error: Uncaught DOMException: Wrong Document Error in %s:%d
Stack trace:
#0 %s(%d): DOM\HTMLDocument->saveHTML(Object(DOM\HTMLDocument))
#1 {main}
  thrown in %s on line %d
