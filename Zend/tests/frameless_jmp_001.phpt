--TEST--
Frameless jmp
--FILE--
<?php
namespace Foo;
function test($needle, $haystack) {
    return in_array($needle, $haystack, true);
}
var_dump(test('foo', ['foo', 'bar']));
?>
--EXPECT--
bool(true)
