--TEST--
DirectoryIterator: Test empty value to DirectoryIterator constructor
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
$it = new DirectoryIterator("");
?>
--EXPECTF--
Fatal error: Uncaught exception 'RuntimeException' with message 'Directory name must not be empty.' in %s:%d
Stack trace:
#0 %s(%d): DirectoryIterator->__construct('')
#1 {main}
  thrown in %s on line %d