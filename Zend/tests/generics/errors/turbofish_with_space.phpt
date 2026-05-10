--TEST--
Errors: whitespace between :: and < is not turbofish
--FILE--
<?php
class C { static function f() {} }
try {
    eval('C:: <int>::f();');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
