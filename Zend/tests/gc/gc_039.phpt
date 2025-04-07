--TEST--
GC 039: Garbage created by replacing argument send by reference
--EXTENSIONS--
mbstring
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
