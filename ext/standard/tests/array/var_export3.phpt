--TEST--
var_export() and classes
--FILE--
<?php
class kake {
    public $mann;
    protected $kvinne;

    function __construct()
    {
        $this->mann = 42;
        $this->kvinne = 43;
    }
}

$kake = new kake;

var_export($kake);
?>
--EXPECTF--
Deprecated: Passing object of class (kake) with no __set_state method is deprecated in %s on line %d
\kake::__set_state(array(
   'mann' => 42,
   'kvinne' => 43,
))
