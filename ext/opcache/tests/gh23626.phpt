--TEST--
GH-23626: OPcache: orphaned temporaries cause leaks and assertions
--CREDITS--
Mrmaxmeier
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
class D {
    public function __construct(public string $name) {}
    public function __clone() { $this->name .= '-clone'; }
    public function __destruct() { echo "destruct {$this->name}\n"; }
}

function test_assert($u) {
    (function () use ($u) {}) ** match (1) { 2 => 3 };
}

function one($o) {
    (clone $o) ** match (1) { 2 => 3 };
}

function two($a, $b) {
    (clone $a) ** ((clone $b) ** match (1) { 2 => 3 });
}

function rope($o) {
    return "x{$o->name}y" . match (1) { 2 => 3 };
}

$a = new D('a');
$b = new D('b');

foreach (['test_assert' => [1], 'one' => [$a], 'two' => [$a, $b], 'rope' => [$a]] as $fn => $args) {
    try {
        $fn(...$args);
    } catch (\UnhandledMatchError $e) {
        echo "$fn: ", $e->getMessage(), "\n";
    }
}

unset($a, $b, $args, $e);
echo "done\n";
?>
--EXPECT--
test_assert: Unhandled match case 1
destruct a-clone
one: Unhandled match case 1
destruct a-clone
destruct b-clone
two: Unhandled match case 1
rope: Unhandled match case 1
destruct b
destruct a
done
