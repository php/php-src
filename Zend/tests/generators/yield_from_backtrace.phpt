--TEST--
Exceptions in linear yield from setup
--FILE--
<?php
function from($off) {
	debug_print_backtrace();
	yield $off + 1;
}

function gen() {
	yield 1;
	debug_print_backtrace();
	yield 2;
	yield from from(2);
	debug_print_backtrace();
}

print "\nImplicit foreach:\n";
foreach (gen() as $v) {
	var_dump($v);
}

print "\nExplicit iterator:\n";
for ($gen = gen(); $gen->valid(); $gen->next()) {
	var_dump($gen->current());
}
?>
--EXPECTF--
Implicit foreach:
int(1)
#0  gen() called at [%s:%d]
int(2)
#0  from(2) called at [%s:%d]
#1  gen() called at [%s:%d]
int(3)
#0  gen() called at [%s:%d]

Explicit iterator:
int(1)
#0  gen()
#1  Generator->next() called at [%s:%d]
int(2)
#0  from(2) called at [%s:%d]
#1  gen()
#2  Generator->next() called at [%s:%d]
int(3)
#0  gen()
#1  Generator->next() called at [%s:%d]

