--TEST--
zend_test_call_with_consumed_args(): multi-arg masks are ignored
--EXTENSIONS--
zend_test
--FILE--
<?php

$result = zend_test_call_with_consumed_args(
    static fn($a, $b) => $a + $b,
    [1, 2],
    3,
);

var_dump($result["consumed_args"]);
var_dump($result["retval"]);

?>
--EXPECT--
int(0)
int(3)
