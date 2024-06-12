--TEST--
GH-9434: new ArrayObject($targetObject) allows setting typed properties to invalid types
--EXTENSIONS--
spl
--FILE--
<?php
class C {
    public int $intProp;
}
$c = new C();
$a = new ArrayObject($c);

try {
    $a['intProp'] = [];
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
var_dump($c);
?>
--EXPECT--
TypeError: Cannot assign array to property C::$intProp of type int
object(C)#1 (0) {
  ["intProp"]=>
  uninitialized(int)
}
