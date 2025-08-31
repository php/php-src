--TEST--
GH-8273 (SplFileObject: key() returns wrong value)
--FILE--
<?php

$file = new SplTempFileObject();

// write to file
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

// read from file
$file->rewind();
while ($file->valid()) {
    echo $file->key(), ': ', $file->fgets();
}
?>
--EXPECT--
0: line 0
1: line 1
2: line 2
3: line 3
4: line 4
