--TEST--
Scoping: type parameters resolve in subsequent bounds (left to right)
--FILE--
<?php
class Box<T : object, U : T> {
    public U $val;
}
$rt = (new ReflectionClass('Box'))->getProperty('val')->getType();
echo $rt->getName(), "\n";
?>
--EXPECT--
object
