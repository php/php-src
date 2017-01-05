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
--EXPECT--
OK
