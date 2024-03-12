--TEST--
$field refers to backing store from either hook
--FILE--
<?php

class Test {
    public $prop {
        get { 
            echo "get\n";
            $field = 'prop';
        }
        set {
            echo "set\n";
            var_dump($field);
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
