--TEST--
$field in different assignments
--FILE--
<?php

class Test {
    public $prop {
        set {
            $field ??= 42;
            var_dump($field);
            $field += 1;
            var_dump($field);
            $field -= 2;
            var_dump($field);
            $field *= 3;
            var_dump($field);
            $field++;
            var_dump($field);
            --$field;
            var_dump($field);
        }
    }
}

$test = new Test;
$test->prop = null;

?>
--EXPECT--
int(42)
int(43)
int(41)
int(123)
int(124)
int(123)
