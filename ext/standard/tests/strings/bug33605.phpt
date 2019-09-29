--TEST--
Bug #33605 (substr_compare crashes)
--FILE--
<?php
try {
    substr_compare("aa", "a", -99999999, -1, 0);
} catch (\Error $e) {
    echo $e->getMessage();
}

?>
--EXPECTF--
The length must be greater than or equal to zero
