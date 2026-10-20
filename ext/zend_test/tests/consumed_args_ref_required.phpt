--TEST--
zend_test_call_with_consumed_args(): ref-required params are not consumed
--EXTENSIONS--
zend_test
--FILE--
<?php

$result = zend_test_call_with_consumed_args(
    static function (&$a) {
        $a = 42;
        return $a;
    },
    [1],
    1,
);

var_dump($result["consumed_args"]);
var_dump($result["retval"]);

?>
--EXPECTF--
Warning: {closure:%s:%d}(): Argument #1 ($a) must be passed by reference, value given in %s on line %d
int(0)
int(42)
