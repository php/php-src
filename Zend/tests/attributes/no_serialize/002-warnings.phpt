--TEST--
#[\NoSerialize]: Warnings test.
--FILE--
<?php

class Base {
    #[\NoSerialize]
    static public $b1 = 'a';

    #[\NoSerialize]
    protected $b2 {
        get => 1;
    }
}
?>
--EXPECTF--
Warning: Static property Base::$b1 is not serializable in %s on line %d

Warning: Virtual property Base::$b2 is not serializable in %s on line %d
