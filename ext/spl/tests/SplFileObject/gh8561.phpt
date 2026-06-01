--TEST--
GH-8561 (SplFileObject: key() and current() unsynchronized after fgets())
--FILE--
<?php
$file = new SplTempFileObject();
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

// Case 1: rewind + fgets, then key/current
$file->rewind();
$file->fgets();
echo "After rewind+fgets: key=" . $file->key() . " current=" . trim($file->current()) . "\n";

// Case 2: multiple fgets
$file->rewind();
$file->fgets();
$file->fgets();
echo "After rewind+fgets+fgets: key=" . $file->key() . " current=" . trim($file->current()) . "\n";

// Case 3: current then fgets
$file->rewind();
$file->current();
$file->fgets();
echo "After current+fgets: key=" . $file->key() . " current=" . trim($file->current()) . "\n";
?>
--EXPECT--
After rewind+fgets: key=1 current=line 1
After rewind+fgets+fgets: key=2 current=line 2
After current+fgets: key=1 current=line 1
