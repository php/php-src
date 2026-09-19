--TEST--
GH-23662 (print_r() results in warning for NAN)
--FILE--
<?php
print_r(NAN);
print_r([NAN]);
?>
--EXPECT--
NANArray
(
    [0] => NAN
)
