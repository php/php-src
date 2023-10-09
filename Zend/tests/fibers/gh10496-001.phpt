--TEST--
Bug GH-10496 001 (Segfault when garbage collector is invoked inside of fiber)
--FILE--
<?php

function x(&$ref) {
	$ref = new class() {
		function __destruct() {
			print "Dtor x()\n";
		}
	};
}
function suspend($x) {
	Fiber::suspend();
}
$f = new Fiber(function() use (&$f) {
	try {
		x($var);
		\ord(suspend(1));
	} finally {
		print "Cleaned\n";
	}
});
$f->start();
unset($f);
gc_collect_cycles();
print "Collected\n";

?>
--EXPECT--
Cleaned
Dtor x()
Collected
