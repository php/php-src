--TEST--
async functions return a Future that await unwraps
--FILE--
<?php
async function answer(): int {
    return 42;
}

$future = answer();
var_dump($future instanceof Async\Future);
var_dump($future->isDone());
var_dump(await $future);
var_dump(await $future);

$fn = new ReflectionFunction('answer');
echo $fn->getReturnType(), "\n";
?>
--EXPECT--
bool(true)
bool(true)
int(42)
int(42)
Async\Future
