--TEST--
Demonstrate that cache_slot optimization is illegal due to cache_slot merging
--FILE--
<?php

class Test {
    public int $prop;

    public function method() {
        // Opcache merges cache slots for both assignments.
        $this->prop = 1;
        try {
            $this->prop = "foobar";
        } catch (TypeError $e) {
            echo $e->getMessage(), "\n";
        }
        var_dump($this->prop);
    }
}

$test = new Test;
$test->method();
$test->method();

?>
--EXPECT--
Cannot assign string to property Test::$prop of type int
int(1)
Cannot assign string to property Test::$prop of type int
int(1)
