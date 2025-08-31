--TEST--
Bug #70228 (memleak if return in finally block)
--FILE--
<?php

function foo() {
    $array = [1, 2, $n = 3];
    foreach ($array as $value) {
        var_dump($value);
        try {
            try {
                foreach ($array as $_) {
                    return str_repeat("a", 2);
                }
            } finally {
                throw new Exception;
            }
        } catch (Exception $e) { }
    }
}

foo();
?>
--EXPECT--
int(1)
int(2)
int(3)
