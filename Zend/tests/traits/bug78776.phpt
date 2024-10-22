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
Fatal error: Cannot make non static method A::createApp() static in class C in %s on line %d
