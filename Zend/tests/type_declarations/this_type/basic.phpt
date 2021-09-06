--TEST--
Basic usage of $this return type
--FILE--
<?php

class Test {
    public function returnsThis(): $this {
        return $this;
    }

    public function returnsStatic(): $this {
        return new static;
    }
}

$test = new Test;
var_dump($test->returnsThis());
try {
    var_dump($test->returnsStatic());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(Test)#1 (0) {
}
Test::returnsStatic(): Return value must be of type $this, Test returned
