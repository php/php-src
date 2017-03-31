--TEST--
Bug #74351 (Wrong reflection on variadic array functions)
--FILE--
<?php
function test1($args) {}
function test2(...$args) {}
function test3($arg, ...$args) {}

$functions = [
    'array_diff',
    'array_diff_uassoc',
    'array_diff_ukey',
    'array_intersect_uassoc',
    'array_intersect_ukey',
    'array_udiff',
    'array_udiff_assoc',
    'array_udiff_uassoc',
    'array_uintersect',
    'array_uintersect_assoc',
    'array_uintersect_uassoc',
    'test1',
    'test2',
    'test3',
];

foreach ($functions as $func) {
    $reflFunc = new ReflectionFunction($func);
    $nbParams = $reflFunc->getNumberOfParameters();
    $nbRequiredParams = $reflFunc->getNumberOfRequiredParameters();
    printf("$func ($nbParams params, $nbRequiredParams required) %s VARIADIC", $reflFunc->isVariadic() ? "IS" : "IS NOT");
    foreach ($reflFunc->getParameters() as $i => $param) {
        if ($param->isVariadic()) {
            printf(" - PARAM #%d IS VARIADIC", $i + 1);
        }
    }
    echo PHP_EOL;
}
?>
--EXPECT--
array_diff (2 params, 2 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_diff_uassoc (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_diff_ukey (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_intersect_uassoc (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_intersect_ukey (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_udiff (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_udiff_assoc (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_udiff_uassoc (4 params, 4 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_uintersect (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_uintersect_assoc (3 params, 3 required) IS VARIADIC - PARAM #2 IS VARIADIC
array_uintersect_uassoc (4 params, 4 required) IS VARIADIC - PARAM #2 IS VARIADIC
test1 (1 params, 1 required) IS NOT VARIADIC
test2 (1 params, 0 required) IS VARIADIC - PARAM #1 IS VARIADIC
test3 (2 params, 1 required) IS VARIADIC - PARAM #2 IS VARIADIC
