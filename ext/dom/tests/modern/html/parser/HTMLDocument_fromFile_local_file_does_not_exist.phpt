--TEST--
DOM\HTMLDocument::createFromFile() - local file that does not exist
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromFile(__DIR__ . "/../foobar");
echo $dom->saveHTML(), "\n";

?>
--EXPECTF--
Warning: DOM\HTMLDocument::createFromFile(%s): Failed to open stream: No such file or directory in %s on line %d

Fatal error: Uncaught Exception: Cannot open file '%s' in %s:%d
Stack trace:
#0 %s(%d): DOM\HTMLDocument::createFromFile('%s')
#1 {main}
  thrown in %s on line %d
