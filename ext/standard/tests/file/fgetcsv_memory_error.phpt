--TEST--
fgetcsv() throws a catchable MemoryError when the requested length cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$fp = fopen('php://memory', 'r+');
fwrite($fp, "a,b,c\n");
rewind($fp);

try {
    fgetcsv($fp, PHP_INT_MAX - 100, ',', '"', '');
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(fgetcsv($fp, 1024, ',', '"', ''));
fclose($fp);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
