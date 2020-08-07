--TEST--
Match expression can be used as a constant expression
--FILE--
<?php

// Here, this test uses strtolower because the locale dependence and change to the string
// ensures PHP has to use a temporary value instead of a literal, to test memory management.
//
// It's already possible to throw from a constant expression (e.g. undeclared constants),
// and match() can be seen as a much shorter way than changed ternary conditions on the same constant expressions.
define('dynamic_value', strtolower('FOO'));
function my_test($invalid = match(dynamic_value) {}) {
    return $invalid;
}

const X = match (dynamic_value) {
    default => 123,
};
echo "X=",X,"\n";
const Y = match (dynamic_value) {
    'a', 'b' => 'default',
    default => dynamic_value . 'd',
};
echo "Y=",Y,"\n";
const Z = match (dynamic_value) {
    'a', 'foo', 'b' => [dynamic_value],
    default => dynamic_value,
};
echo "Z=",json_encode(Z),"\n";

echo json_encode(my_test([dynamic_value])), "\n";
for ($i = 0; $i < 2; $i++) {
    try {
        my_test();
    } catch (UnhandledMatchError $e) {
        echo "Caught {$e->getMessage()} at line {$e->getLine()}\n";
    }
}

?>
--EXPECT--
X=123
Y=food
Z=["foo"]
["foo"]
Caught Unhandled match value of type string at line 9
Caught Unhandled match value of type string at line 9
