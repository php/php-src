--TEST--
Bug #55719 (Argument restriction should come with a more specific error message)
--FILE--
<?php

class Foo
{
}

abstract class Base
{
    abstract public function test(Foo $foo, array $bar, $option = null, $extra = 16777215);
}

class Sub extends Base
{
    public function test(Foo $foo, array $bar, $option = null, $extra = 0xffffff)
    {
    }
}

?>
--EXPECT--
