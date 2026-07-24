--TEST--
GH-17126 (get_class_methods($closure) doesn't return __invoke)
--FILE--
<?php

$closure = fn (string $str) => "hello {$str}";

echo "from object:\n";
$methods = get_class_methods($closure);
sort($methods);
print_r($methods);

echo "from class name:\n";
$methods = get_class_methods('Closure');
sort($methods);
print_r($methods);

echo "unrelated class unaffected:\n";
class NoInvoke { public function foo() {} }
print_r(get_class_methods('NoInvoke'));

?>
--EXPECT--
from object:
Array
(
    [0] => __invoke
    [1] => bind
    [2] => bindTo
    [3] => call
    [4] => fromCallable
    [5] => getCurrent
)
from class name:
Array
(
    [0] => __invoke
    [1] => bind
    [2] => bindTo
    [3] => call
    [4] => fromCallable
    [5] => getCurrent
)
unrelated class unaffected:
Array
(
    [0] => foo
)
