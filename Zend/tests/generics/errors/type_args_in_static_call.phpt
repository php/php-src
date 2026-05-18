--TEST--
Errors: type arguments not allowed before :: (use ::<...> turbofish instead)
--FILE--
<?php
class Foo { static function bar() {} }
try {
    eval('Foo<int>::bar();');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
