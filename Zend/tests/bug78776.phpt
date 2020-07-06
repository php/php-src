--TEST--
Bug #78776: Abstract method implementation from trait does not check "static"
--FILE--
<?php

abstract class A
{
    abstract public function createApp();
}

class B extends A
{
    use C;
}

trait C
{
    public static function createApp()
    {
        echo "You should not be here\n";
    }
}

B::createApp();

?>
--EXPECTF--
Fatal error: Method C::createApp() must not be static to be compatible with overridden method A::createApp() in %s on line %d
