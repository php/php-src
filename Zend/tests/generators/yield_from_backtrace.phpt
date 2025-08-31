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
#0 %s(%d): gen()
int(2)
#0 %s(%d): from(2)
#1 %s(%d): gen()
int(3)
#0 %s(%d): gen()

Explicit iterator:
int(1)
#0 [internal function]: gen()
#1 %s(%d): Generator->next()
int(2)
#0 %s(%d): from(2)
#1 [internal function]: gen()
#2 %s(%d): Generator->next()
int(3)
#0 [internal function]: gen()
#1 %s(%d): Generator->next()
