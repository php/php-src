--TEST--
Awaiting a Future that can never complete throws instead of hanging
--FILE--
<?php
$self = null;
$f = Async\async(function () use (&$self) {
    Async\suspend();
    try {
        await $self;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    Async\suspend();
    return "after suspend";
});
$self = $f;
var_dump(await $f);

$a = null;
$b = Async\async(function () use (&$a) { Async\suspend(); return await $a; });
$a = Async\async(function () use ($b) { return await $b; });
try {
    await $a;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
A task cannot await its own Future
string(13) "after suspend"
Deadlock detected: the awaited Future can never complete
