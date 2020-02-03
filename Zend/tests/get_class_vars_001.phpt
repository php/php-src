--TEST--
get_class_vars(): Simple test
--FILE--
<?php

class A
{
    public $a = 1;
    private $b = 2;
    private $c = 3;
}

class B extends A
{
    public static $aa = 4;
    private static $bb = 5;
    protected static $cc = 6;
}


var_dump(get_class_vars('A'));
var_dump(get_class_vars('B'));

?>
--EXPECT--
array(1) {
  ["a"]=>
  int(1)
}
array(2) {
  ["a"]=>
  int(1)
  ["aa"]=>
  int(4)
}
