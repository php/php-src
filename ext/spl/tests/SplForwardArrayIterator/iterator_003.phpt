--TEST--
Spl\ForwardArrayIterator: Iterator invalid states
--FILE--
<?php

$ht = [1];

$it = new Spl\ForwardArrayIterator($ht);
$it->rewind();
$it->next();

assert(!$it->valid());
try {
    $it->current();
} catch (Throwable $ex) {
    echo "Caught current.\n";
}

try {
    $it->key();
} catch (Throwable $ex) {
    echo "Caught key.\n";
}

try {
    $it->next();
} catch (Throwable $ex) {
    echo "Caught next.\n";
}

?>
--EXPECTF--
Caught current.
Caught key.
Caught next.
