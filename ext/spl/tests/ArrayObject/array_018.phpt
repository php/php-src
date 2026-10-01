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
    echo $e::class, ': ', $e->getMessage(), "\n";
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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

var_dump($foo);

?>
--EXPECTF--
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    ["%0"]=>
    string(3) "Foo"
  }
}
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [""]=>
    string(3) "Foo"
  }
}
