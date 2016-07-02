--TEST--
Don't optimize dynamic call to non-dynamic one if it drops the warning
--FILE--
<?php

function test() {
    ((string) 'extract')(['a' => 42]);
}
test();

?>
--EXPECTF--
Warning: Cannot call extract() dynamically in %s on line %d
