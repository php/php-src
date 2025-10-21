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

#[NoSerialize]
class C extends Base {
    public $a = 'a';
    function __construct(
        #[\NoSerialize]
        protected $c2 {
            get => 1;
        }
    ) {}
}
?>
--EXPECTF--
Warning: Cannot apply #[\NoSerialize] to static property Base::$b1 in %s on line %d

Warning: Cannot apply #[\NoSerialize] to virtual property Base::$b2 in %s on line %d

Warning: Cannot apply #[\NoSerialize] to virtual property C::$c2 in %s on line %d
