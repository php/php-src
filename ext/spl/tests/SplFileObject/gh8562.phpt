--TEST--
Bug GH-8562: SplFileObject: current() returns wrong result after call to next()
--FILE--
<?php
$file = new SplTempFileObject();

// write to file
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

$file->rewind();

$file->next();
echo $file->key(), ': ', $file->current();
$file->next();
echo $file->key(), ': ', $file->current();

?>
--EXPECT--
1: line 1
2: line 2
