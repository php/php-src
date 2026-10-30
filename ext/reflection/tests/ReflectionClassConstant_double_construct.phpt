--TEST--
ReflectionClassConstant double construct call does not leak $name and $class
--FILE--
<?php

class C {
    const FOO = 1;
}

function test(ReflectionClassConstant $r) {
    /* implode() so that the name is not an interned string. */
    $r->__construct(C::class, implode('', ['F', 'O', 'O']));
}

$r = new ReflectionClassConstant(C::class, 'FOO');
for ($i = 0; $i < 10; $i++) {
    test($r);
}

$before = memory_get_usage();
for ($i = 0; $i < 1000; $i++) {
    test($r);
}
$after = memory_get_usage();

var_dump($before === $after);
var_dump($r->name, $r->class);

?>
--EXPECT--
bool(true)
string(3) "FOO"
string(1) "C"
