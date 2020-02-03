--TEST--
get_class_vars(): Testing the scope
--FILE--
<?php

class A
{
    public $a = 1;
    public static $A = 2;

    private $b = 3;
    private static $B = 4;

    protected $c = 5;
    protected static $C = 6;

    public function __construct()
    {
        var_dump(get_class_vars('A'));
    }

    public static function test()
    {
        var_dump(get_class_vars('A'));
    }
}

var_dump(get_class_vars('A'));

new A();

var_dump(A::test());

?>
--EXPECT--
array(2) {
  ["a"]=>
  int(1)
  ["A"]=>
  int(2)
}
array(6) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(3)
  ["c"]=>
  int(5)
  ["A"]=>
  int(2)
  ["B"]=>
  int(4)
  ["C"]=>
  int(6)
}
array(6) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(3)
  ["c"]=>
  int(5)
  ["A"]=>
  int(2)
  ["B"]=>
  int(4)
  ["C"]=>
  int(6)
}
NULL
