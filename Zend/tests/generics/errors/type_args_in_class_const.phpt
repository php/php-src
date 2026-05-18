--TEST--
Errors: type arguments not allowed in expression position (Foo<T>::class)
--FILE--
<?php
class Foo {}
try {
    eval('echo Foo<int>::class;');
} catch (ParseError $e) {
    echo "parse error\n";
}
?>
--EXPECT--
parse error
