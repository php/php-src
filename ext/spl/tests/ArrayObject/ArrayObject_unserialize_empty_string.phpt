--TEST--
ArrayObject: test that you can unserialize a empty string
--CREDITS--
Havard Eide <nucleuz@gmail.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--FILE--
<?php
$a = new ArrayObject(array());
$a->unserialize("");
?>
Done
--EXPECT--
Done
