--TEST--
Bug #63943 (Bad warning text from strpos() on empty needle)
--FILE--
<?php
try {
    strpos("lllllll", '');
} catch (\ErrorException $e) {
    echo $e->getMessage();
}
?>
--EXPECTF--
Empty needle
