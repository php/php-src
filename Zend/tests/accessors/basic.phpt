--TEST--
Basic accessor syntax
--FILE--
<?php

class Test {
    public $prop1 { get; set; }
    public $prop2 {
        get { }
        private set { }
    }
    public $prop3 = 1 { get; private set; }
}

?>
--EXPECT--

