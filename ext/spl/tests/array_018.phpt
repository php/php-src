--TEST--
SPL: ArrayObject and \0
--FILE--
<?php

try
{
	$foo = new ArrayObject();
	$foo->offsetSet("\0", "Foo");
}
catch (Exception $e)
{
	var_dump($e->getMessage());
}

var_dump($foo);

try
{
	$foo = new ArrayObject();
	$data = explode("=", "=Foo");
	$foo->offsetSet($data[0], $data[1]);
}
catch (Exception $e)
{
	var_dump($e->getMessage());
}

var_dump($foo);

?>
===DONE===
--EXPECTF--
string(44) "An offset must not begin with \0 or be empty"
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
string(44) "An offset must not begin with \0 or be empty"
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
===DONE===
--UEXPECTF--
unicode(44) "An offset must not begin with \0 or be empty"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
unicode(44) "An offset must not begin with \0 or be empty"
object(ArrayObject)#%d (1) {
  [u"storage":u"ArrayObject":private]=>
  array(0) {
  }
}
===DONE===
