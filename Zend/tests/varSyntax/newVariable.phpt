--TEST--
Variable as class name for new expression
--FILE--
<?php

$className = 'StdClass';
$array = ['className' => 'StdClass'];
$obj = (object) ['className' => 'StdClass'];

class Test {
    public static $className = 'StdClass';
}
$test = 'Test';
$weird = [0 => (object) ['foo' => 'Test']];

var_dump(new $className);
var_dump(new $array['className']);
var_dump(new $array{'className'});
var_dump(new $obj->className);
var_dump(new Test::$className);
var_dump(new $test::$className);
var_dump(new $weird[0]->foo::$className);

?>
--EXPECTF--
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
object(StdClass)#%d (0) {
}
