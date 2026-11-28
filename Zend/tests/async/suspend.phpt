--TEST--
Async\suspend() yields to other ready tasks
--FILE--
<?php
async function loop(string $name) {
    for ($i = 0; $i < 3; $i++) {
        echo "$name$i ";
        Async\suspend();
    }
}
$a = loop("a");
$b = loop("b");
await $a;
await $b;
echo "\n";
?>
--EXPECT--
a0 b0 a1 b1 a2 b2 
