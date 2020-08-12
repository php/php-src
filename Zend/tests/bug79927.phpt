--TEST--
Bug #79927: Generator doesn't throw exception after multiple yield from iterable
--FILE--
<?php

$generator = (function () {
    yield from [1, 2, 3];
})();

$generator->next();
$generator->next();
try {
    $generator->rewind();
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
echo $generator->current(), "\n";

$generator2 = (function () {
    yield from [];
    yield 4;
})();
$generator2->current();
$generator2->rewind();
echo $generator2->current(), "\n";

?>
--EXPECT--
Cannot rewind a generator that was already run
3
4
