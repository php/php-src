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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Test::__serialize() must return an array
