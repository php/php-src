--TEST--
fread() throws a catchable MemoryError when the requested length cannot fit in the memory limit
--INI--
memory_limit=64M
opcache.enable_cli=0
--FILE--
<?php

$fp = fopen('php://memory', 'r+');
fwrite($fp, 'hello');
rewind($fp);

try {
    fread($fp, PHP_INT_MAX);
} catch (MemoryError $e) {
    echo $e::class . ': ' . $e->getMessage() . "\n";
}

var_dump(fread($fp, 5));
fclose($fp);

?>
--EXPECT--
MemoryError: The resulting string is too large to fit in the configured memory limit
string(5) "hello"
