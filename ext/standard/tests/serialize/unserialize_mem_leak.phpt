--TEST--
Memleaks if unserialize return a self-referenced array/object
--XFAIL--
To fix this, we need change zval_ptr_dtor_nogc to zval_ptr_dtor in free_args, but it will introduce slowdown. mark this XFAIL now. we may find better fix later.
--INI--
report_memleaks=1
--FILE--
<?php
function foo() {
	gc_collect_cycles();
}

$str = 'a:1:{i:0;r:1;}';
foo(unserialize($str));
echo "okey";
?>
--EXPECT--
okey
