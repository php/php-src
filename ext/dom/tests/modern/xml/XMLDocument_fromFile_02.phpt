--TEST--
Dom\XMLDocument::createFromFile 02
--EXTENSIONS--
dom
--FILE--
<?php

Dom\XMLDocument::createFromFile("\\0");

?>
--EXPECTF--
Warning: Dom\XMLDocument::createFromFile(): I/O warning : failed to load external entity "%s" in %s on line %d

Fatal error: Uncaught Exception: Cannot open file '\0' in %s:%d
Stack trace:
#0 %s(%d): Dom\XMLDocument::createFromFile('\\0')
#1 {main}
  thrown in %s on line %d
