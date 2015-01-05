--TEST--
Bug #66286: Incorrect object comparison with inheritance
--FILE--
<?php

abstract class first {
    protected $someArray = array();
}     

class second extends first {    
    protected $someArray = array();        
    protected $someValue = null;

    public function __construct($someValue) {
        $this->someValue = $someValue;
    }
}

$objFirst = new second('123');       
$objSecond = new second('321');       

var_dump ($objFirst == $objSecond);    

?>
--EXPECT--
bool(false)
