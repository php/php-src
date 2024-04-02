--TEST--
DOM\HTMLDocument::createFromFile() - empty path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile("");

?>
--EXPECTF--
Fatal error: Uncaught ValueError: Path cannot be empty in %s:%d
Stack trace:
#0 %s(%d): DOM\HTMLDocument::createFromFile('')
#1 {main}
  thrown in %s on line %d
