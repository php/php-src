--TEST--
RFC example: expected class int and returned integer

--FILE--
<?php
function answer(): int {
    return 42;
}

answer();
?>
DONE
--EXPECT--
DONE
