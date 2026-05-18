--TEST--
Scoping: T from class scope is visible in property types
--FILE--
<?php
class Box<T : object> {
    public T $value;
}
$rt = (new ReflectionClass('Box'))->getProperty('value')->getType();
echo $rt->getName(), "\n";
?>
--EXPECT--
object
