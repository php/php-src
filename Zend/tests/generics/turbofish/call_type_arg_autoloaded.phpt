--TEST--
Turbofish: a class type argument is autoloaded before the bound check
--FILE--
<?php
spl_autoload_register(function ($c) {
    if ($c === 'Iface') { eval('interface Iface {}'); }
    if ($c === 'MyColl') { eval('class MyColl implements Countable { public function count(): int { return 0; } }'); }
});

function f<T: object>(): void { echo "f called\n"; }
function g<T: Countable>(): void { echo "g called\n"; }

f::<Iface>();
g::<MyColl>();
echo "ok\n";
?>
--EXPECT--
f called
g called
ok
