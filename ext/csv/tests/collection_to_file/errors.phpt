--TEST--
Test Csv\collection_to_file(): error conditions
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_errors.csv';

try {
    Csv\collection_to_file($file, [['a', 'b']], '');
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
var_dump(file_exists($file));

try {
    Csv\collection_to_file(__DIR__ . '/no_such_dir/out.csv', [['a', 'b']]);
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
    Csv\collection_to_file($file, [['a', 'b'], 'not an array']);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
/* Rows written before the failure remain in the file */
echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));

try {
    Csv\collection_to_file($file, [['a', 'b'], ['c']]);
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_errors.csv');
?>
--EXPECTF--
ValueError: Csv\collection_to_file(): Argument #3 ($delimiter) must not be empty
bool(false)
Error: Csv\collection_to_file(): Failed to open stream: No such file or directory
TypeError: Element 1 of the collection must be an array
a,b<CRLF>
ValueError: Element 1 of the collection contains 1 fields compared to 2 fields on previous rows
a,b<CRLF>
