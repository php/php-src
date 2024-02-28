--TEST--
Passing Closure as parameter to an non-existent function
--FILE--
<?php

class foo {
    public static function __callstatic($x, $y) {
        var_dump($x,$y);
        return 1;
    }

    public static function teste() {
        return foo::x(function &($a=1,$b) { });
    }
}

var_dump(call_user_func(array('foo', 'teste')));

?>
--EXPECTF--
Deprecated: Optional parameter $a declared before required parameter $b is implicitly treated as a required parameter in %s on line %d
string(1) "x"
array(1) {
  [0]=>
  object(Closure)#%d (2) {
    ["name"]=>
    string(%d) "{closure:%s:%d}"
    ["parameter"]=>
    array(2) {
      ["$a"]=>
      string(10) "<required>"
      ["$b"]=>
      string(10) "<required>"
    }
  }
}
int(1)
