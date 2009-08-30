--TEST--
ArrayObject: test that you cannot unserialize a empty string
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
$a = new ArrayObject(array());
$a->unserialize("");
?>
--EXPECTF--
Fatal error: Uncaught exception 'UnexpectedValueException' with message 'Empty serialized string cannot be empty' in %s.php:%d
Stack trace:
#0 %s(%d): ArrayObject->unserialize('')
#1 {main}
  thrown in %s.php on line %d
