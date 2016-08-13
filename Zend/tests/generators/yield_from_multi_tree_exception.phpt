--TEST--
yield from on multiple trees needing merge
--FILE--
<?php

function from($levels) {
	foreach (range(0, 2 << $levels) as $v) {
		yield $v;
		if ($v == (1 << ($levels - 1)) - 2) {
			throw new Exception();
		}
	}
}

function gen($gen, $level) {
	yield from $gen;
}

$levels = 5;

print "$levels levels\n\n";

$all = array();
$all[] = $gens[0][0] = from($levels);

for ($level = 1; $level < $levels; $level++) {
	for ($i = 0; $i < (1 << $level); $i++) {
		$all[] = $gens[$level][$i] = gen($gens[$level-1][$i >> 1], $level);
	}
}

for ($i = 0; $i < 2; $i++) { 
	try {
		foreach ($all as $gen) {
			var_dump($gen->current());
			$gen->next();
			if (!$gen->valid()) {
				break;
			}
		}
	} catch(Exception $e) {
		print "$e\n";
		unset($all[array_search($gen, $all)]);
	}
}
?>
--EXPECTF--
5 levels

int(0)
int(1)
int(2)
int(3)
int(4)
int(5)
int(6)
int(7)
int(8)
int(9)
int(10)
int(11)
int(12)
int(13)
int(14)
Exception in %s:%d
Stack trace:
#0 %s(%d): from(5)
#1 %s(%d): gen(Object(Generator), 1)
#2 %s(%d): gen(Object(Generator), 2)
#3 [internal function]: gen(Object(Generator), 3)
#4 %s(%d): Generator->next()
#5 {main}
ClosedGeneratorException: Generator yielded from aborted, no return value available in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator), 1)
#1 %s(%d): Generator->current()
#2 {main}

