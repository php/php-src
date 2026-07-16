--TEST--
Extension methods: basic resolution, receiver binding, real-method precedence
--FILE--
<?php
class Money {
    public function __construct(public int $cents) {}
    public function label(): string { return "real"; }
}

extension Money $m {
    public function dollars(): float { return $m->cents / 100; }
    public function label(): string { return "extension"; } // must lose to real method
}

extension \DateTimeImmutable $d {
    public function isWeekend(): bool {
        return in_array((int)$d->format('N'), [6, 7], true);
    }
}

$m = new Money(1250);
var_dump($m->dollars());
var_dump($m->label());
var_dump((new DateTimeImmutable('2026-07-11'))->isWeekend());
try { $m->missing(); } catch (Error $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
float(12.5)
string(4) "real"
bool(true)
Call to undefined method Money::missing()
