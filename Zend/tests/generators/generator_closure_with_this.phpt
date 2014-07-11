--TEST--
Non-static closures can be generators
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
--EXPECT--
object(Test)#1 (0) {
}
