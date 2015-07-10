--TEST--
Ignoring return inside loop using finally
--FILE--
<?php

function foo() {
    $array = [1, 2, $n = 3];
    foreach ($array as $value) {
        var_dump($value);
        try {
            try {
                foreach ($array as $_) {
                    return;
                }
            } finally {
                throw new Exception;
            }
        } catch (Exception $e) { }
    }
}

foo();
?>
===DONE===
--EXPECT--
int(1)
int(2)
int(3)
===DONE===
