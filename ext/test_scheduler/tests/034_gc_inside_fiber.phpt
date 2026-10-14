--TEST--
Bug GH-10496 001 (Segfault when garbage collector is invoked inside of fiber) — under test_scheduler
--DESCRIPTION--
The expected output departs from the upstream
Zend/tests/fibers/gh10496-001.phpt. Under a scheduler a fiber parked at
Fiber::suspend() is not collected by gc_collect_cycles() when the cycle runs
through its body's closure: the closure sits in the coroutine's fcall, and a
parked fiber keeps its coroutine out of the collector's view, as the scheduler
holds references to it the collector cannot count. The fiber goes when the
scheduler ends it at shutdown, so "Cleaned" and the destructor's output follow
"Collected". The scheduler RFC lists this behavior change as "A fiber parked
in Fiber::suspend() is ended by the scheduler, not collected".
--EXTENSIONS--
test_scheduler
--INI--
test_scheduler.enable=1
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
Collected
Cleaned
Dtor x()
