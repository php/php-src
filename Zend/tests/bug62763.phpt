--TEST--
Bug #62763 (register_shutdown_function and extending class)
--FILE--
<?php
class test1 {
    public function __construct() {
        register_shutdown_function(array($this, 'shutdown'));
    }
    public function shutdown() {
        exit(static::class . '::' . __FUNCTION__ . "\n");
    }
}

class test2 extends test1 {
    public function __destruct() {
        exit(static::class . '::' . __FUNCTION__ . "\n");
    }
}
new test1;
new test2;
?>
--EXPECT--
test1::shutdown
test2::shutdown
test2::__destruct
