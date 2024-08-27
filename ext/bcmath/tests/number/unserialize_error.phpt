--TEST--
BcMath\Number unserialize error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    unserialize('O:13:"BcMath\Number":1:{s:5:"value";s:1:"a";}');
} catch (Error $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Invalid serialization data for BcMath\Number object
