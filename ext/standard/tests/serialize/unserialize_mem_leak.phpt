--TEST--
Memleaks if unserialize return a self-referenced array/object
--INI--
report_memleaks=1
--FILE--
<?php
function foo() {
    gc_collect_cycles();
}

$str = 'a:1:{i:0;R:1;}';
foo(unserialize($str));
$str = 'a:1:{i:0;r:1;}';
foo(unserialize($str));
echo "okey";
?>
--EXPECTF--
Warning: unserialize(): Error at offset 13 of 14 bytes in %s on line %d
okey
