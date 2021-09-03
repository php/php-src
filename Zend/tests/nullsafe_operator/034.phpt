--TEST--
Test nullsafe operator on delayed dim
--FILE--
<?php

$arr = [
    'foo' => null,
    'bar' => [
        'baz' => null,
    ],
];

var_dump($arr['foo']?->something);
var_dump($arr['invalid']?->something);

var_dump($arr['bar']['baz']?->something);
var_dump($arr['bar']['invalid']?->something);

?>
--EXPECTF--
NULL

Warning: Undefined array key "invalid" in %s.php on line 11
NULL
NULL

Warning: Undefined array key "invalid" in %s.php on line 14
NULL
