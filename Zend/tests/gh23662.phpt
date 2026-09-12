--TEST--
GH-23662 (print_r() results in warning for NAN)
--FILE--
<?php
print_r([NAN]);
?>
--EXPECT--
Array
(
    [0] => NAN
)
