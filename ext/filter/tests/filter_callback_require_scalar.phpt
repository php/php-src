--TEST--
FILTER_CALLBACK with explicit FILTER_REQUIRE_SCALAR
--EXTENSIONS--
filter
--FILE--
<?php
function test($var) {
    $callback = function ($var) {
        return $var;
    };
    return filter_var($var, FILTER_CALLBACK, ['options' => $callback, 'flags' => FILTER_REQUIRE_SCALAR]);
}
var_dump(test('test'));
var_dump(test(['test']));
?>
--EXPECT--
string(4) "test"
bool(false)
