--TEST--
Variable as class name for new expression
--FILE--
<?php

$className = 'stdClass';
$array = ['className' => 'stdClass'];
$obj = (object) ['className' => 'stdClass'];

class Test {
    public static $className = 'stdClass';
}
$test = 'Test';
$weird = [0 => (object) ['foo' => 'Test']];

var_dump(new $className);
var_dump(new $array['className']);
var_dump(new $obj->className);
var_dump(new Test::$className);
var_dump(new $test::$className);
var_dump(new $weird[0]->foo::$className);

?>
--EXPECTF--
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
object(stdClass)#%d (0) {
}
