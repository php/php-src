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

$p = 0;
$di->seek($o+1);
while ($di->valid()) {
    $p++;
    $di->next();
}

var_dump($n !== $m, $m === $o, $p === 0);
?>
===DONE===
--EXPECTF--
With seek(2) we get %d
With seek(0) we get %d
Without seek we get %d
bool(true)
bool(true)
bool(true)
===DONE===
