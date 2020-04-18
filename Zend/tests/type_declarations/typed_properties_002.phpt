--TEST--
Test typed properties error condition (read uninitialized)
--FILE--
<?php
$thing = new class() {
    public int $int;
};

var_dump($thing->int);
?>
--EXPECTF--
Fatal error: Uncaught Error: Typed property class@anonymous::$int must not be accessed before initialization in %s:6
Stack trace:
#0 {main}
  thrown in %s on line 6
