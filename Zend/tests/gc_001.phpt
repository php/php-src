--TEST--
GC 001: gc_enable()/gc_diable()/gc_enabled()
--FILE--
<?php
gc_disable();
var_dump(gc_enabled());
gc_enable();
var_dump(gc_enabled());
?>
--EXPECT--
bool(false)
bool(true)
