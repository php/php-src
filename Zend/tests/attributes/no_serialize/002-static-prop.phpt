--TEST--
#[\NoSerialize]: Error on static prop.
--FILE--
<?php

class Base {
    #[\NoSerialize]
    static public $b1 = 'a';
}

?>
--EXPECTF--
Fatal error: Cannot apply #[\NoSerialize] to static property Base::$b1 in %s on line %d
