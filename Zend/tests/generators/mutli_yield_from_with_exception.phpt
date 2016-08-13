--TEST--
Multiple yield from on a same Generator throwing an Exception
--FILE--
<?php
function from() {
	yield 1;
	throw new Exception();
}

function gen($gen) {
	try {
		var_dump(yield from $gen);
	} catch (Exception $e) { print "Caught exception!\n$e\n"; }
}

$gen = from();
$gens[] = gen($gen);
$gens[] = gen($gen);

foreach ($gens as $g) {
	$g->current(); // init.
}

do {
	foreach ($gens as $i => $g) {
		print "Generator $i\n";
		var_dump($g->current());
		$g->next();
	}
} while($gens[0]->valid());
?>
--EXPECTF--
Generator 0
int(1)
Caught exception!
Exception in %s:%d
Stack trace:
#0 %s(%d): from()
#1 [internal function]: gen(Object(Generator))
#2 %s(%d): Generator->next()
#3 {main}
Generator 1
Caught exception!
ClosedGeneratorException: Generator yielded from aborted, no return value available in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator))
#1 %s(%d): Generator->current()
#2 {main}
NULL
