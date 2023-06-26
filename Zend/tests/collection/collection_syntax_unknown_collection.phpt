--TEST--
Collection with unsupported structure type
--FILE--
<?php
collection(Foobar) Articles(int => Article)
{
}
?>
--EXPECTF--
Fatal error: Collection data structure must be Seq or Dict, Foobar given in %s on line %d
