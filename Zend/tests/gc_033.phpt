--TEST--
GC 033: Crash in GC while run with phpspec
--INI--
zend.enable_gc = 1
--FILE--
<?php
$a = new stdClass();
$a->a = array();
$a->a[0] = new Stdclass();
$a->a[0]->a = $a;
$a->a[1] = &$a->a;

/* remove the self-reference array out of roots */
gc_collect_cycles();

/* do unset */
unset($a);

/* let's full the gc roots */
for ($i=0; $i<9999; $i++) {
	    $b = range(0, 1);
		    $b[0] = &$b;
		    unset($b);
}

/* then $a will be freed, but $a->a[0] is not. reference to a freed $a */
var_dump(gc_collect_cycles());
?>
--EXPECT--
int(10002)
