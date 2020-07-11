--TEST--
Bug #79475 ([JIT] func_get_args() assertion violation)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    $args = func_get_args();
    $args[] = "bar";
}
foo();
echo "done\n";
?>
--EXPECT--
done
