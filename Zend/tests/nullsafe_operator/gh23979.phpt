--TEST--
GH-23979 (Nullsafe operator must not flush delayed oplines of an enclosing function)
--FILE--
<?php
function test($name) {
    $arr = ['foo' => 'bar'];
    return ${$name}[(function () {
        return A . B?->prop;
    })()];
}
const A = 'foo';
const B = null;
var_dump(test('arr'));
?>
--EXPECT--
string(3) "bar"
