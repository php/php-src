--TEST--
Tasks waiting on delay() run concurrently and interleave
--FILE--
<?php
use function Async\delay;

async function worker(string $name, array $delays): string {
    foreach ($delays as $i => $d) {
        await delay($d);
        echo "$name step $i\n";
    }
    return $name;
}

$start = hrtime(true);
$a = worker("A", [0.02, 0.04]);
$b = worker("B", [0.03, 0.04]);
var_dump($a->isDone(), $b->isDone());
var_dump(await $b, await $a);
$elapsed = (hrtime(true) - $start) / 1e9;
var_dump($elapsed >= 0.07 && $elapsed < 0.14);
?>
--EXPECT--
bool(false)
bool(false)
A step 0
B step 0
A step 1
B step 1
string(1) "B"
string(1) "A"
bool(true)
