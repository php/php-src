--TEST--
Overriding a static property where both declarations have a doc block does not leak memory
--FILE--
<?php
trait MyTrait {
    /**
     * trait comment
     */
    static $property;
}

class MyClass {
    use MyTrait;

    /**
     * class comment
     */
    static $property;
}
?>
--EXPECT--
