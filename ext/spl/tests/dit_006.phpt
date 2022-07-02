--TEST--
SPL: DirectoryIterator and seek
--FILE--
<?php
$di = new DirectoryIterator(__DIR__."/..");
$di->seek(2);

$n = 0;
while ($di->valid()) {
    $n++;
    $di->next();
}

echo "With seek(2) we get $n\n";
$di->seek(0);

$m = 0;
while ($di->valid()) {
    $m++;
    $di->next();
}
echo "With seek(0) we get $m\n";

$o = 0;
$di->rewind();
while ($di->valid()) {
    $o++;
    $di->next();
}

echo "Without seek we get $o\n";

try {
    $p = 0;
    $di->seek($o+1);
    $p = 1;
} catch (\OutOfBoundsException $ex) {
    echo $ex->getMessage() . PHP_EOL;
}

var_dump($n !== $m, $m === $o, $p === 0);
?>
--EXPECTF--
With seek(2) we get %d
With seek(0) we get %d
Without seek we get %d
Seek position %d is out of range
bool(true)
bool(true)
bool(true)
