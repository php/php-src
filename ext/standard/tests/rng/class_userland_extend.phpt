--TEST--
Test class: userland implemented extended classes.
--FILE--
<?php
class XorShift128PlusEx extends \RNG\XorShift128Plus
{
    public function next(): int
    {
        return parent::next() + 1;
    }

    public function next64(): int
    {
        return parent::next() + 1;
    }
}

$origin = new \RNG\XorShift128Plus(12345);
$extend = new XorShift128PlusEx(12345);

for ($i = 0; $i < 100000; $i++) {
    $origin_next = $origin->next();
    $extend_next = $extend->next();

    if (($origin_next + 1) !== $extend_next) {
        die("NG, userland extended class is broken.");
    }
}

die('OK, userland implementation extended class works correctly.');
?>
--EXPECT--
OK, userland implementation extended class works correctly.
