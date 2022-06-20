--TEST--
Bug GH-8561: SplFileObject: key() and current() unsinchronized after call to fgets()
--FILE--
<?php
$file = new SplTempFileObject();

// write to file
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

$file->rewind();

// read first line
$file->fgets();

// where am I?
echo $file->key(), ': ', $file->current();

?>
--EXPECT--
1: line 1
