--TEST--
Don't try to unlink use chain of QM_ASSIGN without result use
--FILE--
<?php
function test($arg) {
    var_dump(((int)$arg)-0);
}
test(1);
?>
--EXPECT--
int(1)
