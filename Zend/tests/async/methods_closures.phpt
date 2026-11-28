--TEST--
async methods, closures and arrow functions
--FILE--
<?php
class Counter {
    private int $count = 0;

    public async function add(int $by = 1): int {
        await Async\delay(0);
        return $this->count += $by;
    }

    public static async function make(): static {
        return new static;
    }
}

$c = await Counter::make();
var_dump(get_class($c));
await $c->add();
var_dump(await $c->add(5));

$factor = 3;
$arrow = async fn(int $x): int => $x * $factor;
var_dump(await $arrow(14));

$closure = async function (int ...$xs) use ($arrow): int {
    $sum = 0;
    foreach ($xs as $x) {
        $sum += await $arrow($x);
    }
    return $sum;
};
var_dump(await $closure(1, 2, 3));

$static = static async fn() => isset($this);
var_dump(await $static());

// Parameters are shared with the task body.
async function byRef(array &$log) {
    $log[] = "start";
    await Async\delay(0);
    $log[] = "end";
}
$log = [];
$f = byRef($log);
var_dump($log);
await $f;
var_dump($log);
?>
--EXPECT--
string(7) "Counter"
int(6)
int(42)
int(18)
bool(false)
array(1) {
  [0]=>
  string(5) "start"
}
array(2) {
  [0]=>
  string(5) "start"
  [1]=>
  string(3) "end"
}
