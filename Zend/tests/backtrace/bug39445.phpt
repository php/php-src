--TEST--
Bug #39445 (Calling debug_backtrace() in the __toString() function produces a crash)
--FILE--
<?php
class test {
    public function __toString() {
        debug_backtrace();
        return 'lowercase';
    }
}

    $test = new test();
    echo strtoupper($test);
?>
--EXPECT--
LOWERCASE
