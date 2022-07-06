--TEST--
FE_FETCH op2 is a def and needs special live range handling
--FILE--
<?php
try {
    foreach (["test"] as $k => func()[]) {}
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Call to undefined function func()
