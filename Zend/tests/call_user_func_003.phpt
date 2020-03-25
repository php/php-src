--TEST--
Testing call_user_func() with closures
--FILE--
<?php

$foo = function() {
    static $instance;

    if (is_null($instance)) {
        $instance = function () {
            return 'OK!';
        };
    }

    return $instance;
};

var_dump(call_user_func(array($foo, '__invoke'))->__invoke());
var_dump(call_user_func(function() use (&$foo) { return $foo; }, '__invoke'));

?>
--EXPECTF--
string(3) "OK!"
object(Closure)#%d (1) {
  ["static"]=>
  array(1) {
    ["instance"]=>
    object(Closure)#%d (0) {
    }
  }
}
