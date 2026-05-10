--TEST--
Traits: trait declares its own type parameter
--FILE--
<?php
trait Holder<T : object> {
    public T $value;
    public function get(): T { return $this->value; }
}
class Box {
    use Holder<object>;
}
$rc = new ReflectionClass('Box');
echo $rc->getProperty('value')->getType()->getName(), "\n";
echo $rc->getMethod('get')->getReturnType()->getName(), "\n";
?>
--EXPECT--
object
object
