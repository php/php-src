--TEST--
$this->prop refers to backing store from either hook
--FILE--
<?php

class Test {
    public $prop {
        get {
            echo "get\n";
            $this->prop = 'prop';
        }
        set {
            echo "set\n";
            var_dump($this->prop);
        }
    }
}

$test = new Test;
$test->prop;
$test->prop = 42;

?>
--EXPECT--
get
set
string(4) "prop"
