--TEST--
Bug #21094 (set_error_handler not accepting methods)
--FILE--
<?php
class test {
    function hdlr($errno, $errstr, $errfile, $errline) {
        printf("[%d] errstr: %s, errfile: %s, errline: %d\n", $errno, $errstr, $errfile, $errline, $errstr);
    }
}

set_error_handler(array(new test(), "hdlr"));

trigger_error("test");
?>
--EXPECTF--
[1024] errstr: test, errfile: %s, errline: %d
