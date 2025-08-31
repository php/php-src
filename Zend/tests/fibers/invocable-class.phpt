--TEST--
Reference to invocable class retained while running
--FILE--
<?php

class Test {
    public function __invoke() {
        $GLOBALS['test'] = null;
        var_dump($this);
        try {
            Fiber::suspend();
        } finally {
            var_dump($this);
        }
    }
}

$test = new Test;
$fiber = new Fiber($test);
$fiber->start();

?>
--EXPECTF--
object(Test)#%d (0) {
}
object(Test)#%d (0) {
}
