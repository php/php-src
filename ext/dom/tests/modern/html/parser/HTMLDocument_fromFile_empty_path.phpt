--TEST--
Dom\HTMLDocument::createFromFile() - empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromFile("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Path must not be empty in %s:%d
Stack trace:
#0 %s(%d): Dom\HTMLDocument::createFromFile('')
#1 {main}
  thrown in %s on line %d
