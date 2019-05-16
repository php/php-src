--TEST--
Exceptions in linear yield from setup
--FILE--
<?php
function from($off) {
	try {
		yield $off + 1;
	} catch (Exception $e) { print "catch in from()\n$e\n"; }
	yield $off + 2;
}

function gen() {
	try {
		yield "gen" => 0;
	} catch (Exception $e) { print "catch in gen()\n$e\n"; }
	try {
		yield from from(0);
	} catch (Exception $e) { print "catch in gen()\n$e\n"; }
	yield from from(2);
}

$i = 0;
try {
	for ($gen = gen(); $gen->valid(); $gen->throw(new Exception((string) $i++))) {
		var_dump($gen->current());
	}
} catch (Exception $e) { print "catch in {main}\n$e\n"; }

var_dump($gen->valid());

?>
--EXPECTF--
int(0)
catch in gen()
Exception: 0 in %s:%d
Stack trace:
#0 {main}
int(1)
catch in from()
Exception: 1 in %s:%d
Stack trace:
#0 {main}
int(2)
catch in gen()
Exception: 2 in %s:%d
Stack trace:
#0 {main}
int(3)
catch in from()
Exception: 3 in %s:%d
Stack trace:
#0 {main}
int(4)
catch in {main}
Exception: 4 in %s:%d
Stack trace:
#0 {main}
bool(false)
