--TEST--
GC 039: Garbage created by replacing argument send by reference
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
zend.enable_gc = 1
--FILE--
<?php
$out = new stdClass;
$out->x = $out;
mb_parse_str("a=b", $out);
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(1)
