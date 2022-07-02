--TEST--
Bug #72119 (Interface declaration compatibility regression with default values)
--FILE--
<?php
interface Foo {
    public function bar(array $baz = null);
}

class Hello implements Foo {
    public function bar(array $baz = [])
    {

    }
}
echo "OK\n";
?>
--EXPECTF--
Fatal error: Declaration of Hello::bar(array $baz = []) must be compatible with Foo::bar(?array $baz = null) in %s on line %d
