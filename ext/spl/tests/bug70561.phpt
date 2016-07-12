--TEST--
Bug #70561 (DirectoryIterator::seek should throw OutOfBoundsException)
--FILE--
<?php
$di = new DirectoryIterator(__DIR__ . '/..');

$cnt = 0;
$di->rewind();
while ($di->valid()) {
    $cnt++;
    $di->next();
}

try {
    $di->seek($cnt+1);
} catch (OutOfBoundsException $ex) {
    echo $ex->getMessage() . PHP_EOL;
}
echo "Is valid? " . (int) $di->valid() . PHP_EOL;
?>
--EXPECTF--
Seek position %d is out of range
Is valid? 0
