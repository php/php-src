--TEST--
Bug #33605 (substr_compare crashes)
--FILE--
<?php
try {
    substr_compare("aa", "a", -99999999, -1, 0);
} catch (\ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
substr_compare(): Argument #4 ($length) must be greater than or equal to 0
