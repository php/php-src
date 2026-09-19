--TEST--
Non-static closures can be generators — under test_scheduler
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
--FILE--
<?php

class Test {
    public function getGenFactory() {
        return function() {
            yield $this;
        };
    }
}

$genFactory = (new Test)->getGenFactory();
var_dump($genFactory()->current());

?>
--EXPECTF--
object(Test)#%d (0) {
}
