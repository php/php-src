--TEST--
Bug #24436 (isset() and empty() produce errors with non-existent variables in objects)
--FILE--
<?php
class test {
    function __construct() {
        if (empty($this->test[0][0])) { print "test1";}
        if (!isset($this->test[0][0])) { print "test2";}
    }
}

$test1 = new test();
?>
--EXPECT--
test1test2
