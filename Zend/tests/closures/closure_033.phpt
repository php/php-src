--TEST--
Closure 033: Dynamic closure property and private function
--FILE--
<?php

class Test {
    public $func;
    function __construct() {
        $this->func = function() {
            echo __METHOD__ . "()\n";
        };
    }
    private function func() {
        echo __METHOD__ . "()\n";
    }
}

$o = new Test;
$f = $o->func;
$f();
$o->func();

?>
===DONE===
--EXPECTF--
{closure:%s:%d}()

Fatal error: Uncaught Error: Call to private method Test::func() from global scope in %s:%d
Stack trace:
#0 {main}
  thrown in %sclosure_033.php on line %d
