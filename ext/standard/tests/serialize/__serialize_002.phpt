--TEST--
__serialize() mechanism (002): TypeError on invalid return type
--FILE--
<?php

class Test {
    public function __serialize() {
        return $this;
    }
}

try {
    serialize(new Test);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Test::__serialize() must return an array
