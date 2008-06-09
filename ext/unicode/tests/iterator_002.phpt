--TEST--
Unicode: Iterator and key()
--FILE--
<?php

foreach(new TextIterator("123") as $pos => $char)
{
	var_dump($pos);
	var_dump($char);
}

foreach(new ReverseTextIterator("123") as $pos => $char)
{
	var_dump($pos);
	var_dump($char);
}

?>
===DONE===
--EXPECTF--
int(0)
unicode(1) "1"
int(1)
unicode(1) "2"
int(2)
unicode(1) "3"
int(0)
unicode(1) "3"
int(1)
unicode(1) "2"
int(2)
unicode(1) "1"
===DONE===
