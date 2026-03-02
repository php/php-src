--TEST--
Exception during break 2
--FILE--
<?php

class A {
    public $a = 1;
    public $b = 2;

    public function __destruct() {
        throw new Exception;
    }
}

function foo() {
    foreach ([0] as $_) {
        foreach (new A as $value) {
            try {
                break 2;
            } catch (Exception $e) {
                echo "catch\n";
            } finally {
                echo "finally\n";
            }
        }
    }
}

try {
    foo();
} catch (Exception $e) {
    echo "outer catch\n";
}
?>
--EXPECT--
finally
outer catch
