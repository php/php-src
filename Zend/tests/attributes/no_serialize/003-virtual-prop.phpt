--TEST--
#[\NoSerialize]: Error on virtual prop.
--FILE--
<?php

class Base {
    #[\NoSerialize]
    protected $b2 {
        get => 1;
    }
}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NoSerialize] to virtual property Base::$b2 in %s on line %d
