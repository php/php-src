--TEST--
unserialize allowed_classes_callback blocking unserialize
--FILE--
<?php

class TestClass {
}

$serialized = serialize(
    [
        new TestClass(),
        new TestClass(),
    ]
);

$dummy = unserialize(
    $serialized,
    [
        'allowed_classes_callback' => function ($className) {
            return 0;
        }
    ]
);

?>
--EXPECTF--
Fatal error: Uncaught TypeError: "allowed_classes_callback" must return bool, int given in %s__serialize_009.php:%d
Stack trace:
#0 %s__serialize_009.php(%s): unserialize('a:2:{i:0;O:9:"T...', Array)
#1 {main}
  thrown in %s__serialize_009.php on line %d
