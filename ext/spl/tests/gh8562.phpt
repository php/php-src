--TEST--
GH-8562 (SplFileObject::current() returns wrong result after call to next())
--FILE--
<?php
$file = new SplTempFileObject();
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

$file->rewind();
$file->next();
echo "After rewind+next: key=" . $file->key() . " current=" . trim($file->current()) . "\n";

$file->rewind();
$file->next();
$file->next();
echo "After rewind+next+next: key=" . $file->key() . " current=" . trim($file->current()) . "\n";

$file->rewind();
$file->current();
$file->next();
echo "After current+next: key=" . $file->key() . " current=" . trim($file->current()) . "\n";
?>
--EXPECT--
After rewind+next: key=1 current=line 1
After rewind+next+next: key=2 current=line 2
After current+next: key=1 current=line 1
