--TEST--
GH-15938
--FILE--
<?php

$dyn = 'a';
$$dyn = str_repeat('a', 10);
try {
$$dyn .= new class {
    function __toString() {
        global $dyn;
        unset($$dyn);
        return str_repeat('c', 10);
    }
};
} catch (Exception $e) {}

var_dump(get_defined_vars()['a']);

?>
--EXPECT--
string(20) "aaaaaaaaaacccccccccc"
