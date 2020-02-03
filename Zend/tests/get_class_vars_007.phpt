--TEST--
get_class_vars(): Testing with static properties
--FILE--
<?php

class A
{
    public static $a, $aa;
    private static $b, $bb;
    protected static $c, $cc;

    public static function test()
    {
        var_dump(get_class_vars(__CLASS__));
    }
}

var_dump(get_class_vars('A'));
var_dump(A::test());

?>
--EXPECT--
array(2) {
  ["a"]=>
  NULL
  ["aa"]=>
  NULL
}
array(6) {
  ["a"]=>
  NULL
  ["aa"]=>
  NULL
  ["b"]=>
  NULL
  ["bb"]=>
  NULL
  ["c"]=>
  NULL
  ["cc"]=>
  NULL
}
NULL
