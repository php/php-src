--TEST--
FE_FETCH op2 is a def and needs special live range handling
--FILE--
<?php
try {
    foreach (["test"] as $k => func()[]) {}
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Call to undefined function func()
