--TEST--
Exceptions thrown in async functions are rethrown by await
--FILE--
<?php
async function fail(string $msg) {
    await Async\delay(0);
    throw new RuntimeException($msg);
}

async function rethrow() {
    try {
        await fail("inner");
    } catch (RuntimeException $e) {
        throw new LogicException("outer", previous: $e);
    }
}

try {
    await rethrow();
} catch (LogicException $e) {
    echo $e->getMessage(), " <- ", $e->getPrevious()->getMessage(), "\n";
}

$f = fail("sync");
var_dump($f->isDone());
for ($i = 0; $i < 2; $i++) {
    try {
        await $f;
    } catch (RuntimeException $e) {
        echo "caught ", $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
outer <- inner
bool(false)
caught sync
caught sync
