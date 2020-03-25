--TEST--
Bug #24436 (isset()/empty() produce errors with non-existent variables in classes)
--INI--
error_reporting=2047
--FILE--
<?php
class test {
        function __construct() {
                if (empty($this->test[0][0])) { print "test1\n";}
                if (!isset($this->test[0][0])) { print "test2\n";}
                if (empty($this->test)) { print "test1\n";}
                if (!isset($this->test)) { print "test2\n";}
        }
}

$test1 = new test();
?>
--EXPECT--
test1
test2
test1
test2
