--TEST--
GC 027: GC and properties of internal classes
--FILE--
<?php
try {
	throw new Exception();
} catch (Exception $e) {
	gc_collect_cycles();
}
echo "ok\n";
--EXPECT--
ok
