--TEST--
Immediate constant access on new anonymous class object
--FILE--
<?php

echo new class {
    const C = 'constant';
}::C;

?>
--EXPECT--
constant
