--TEST--
Various fgetcsv() error conditions
--FILE--
<?php

$file_name = __DIR__ . '/fgetcsv_error_conditions.csv';
$file_handle = fopen($file_name, 'r');

$length = 1024;
$delimiter = ',';
$enclosure = '"';

echo 'fgetcsv() with negative length' . \PHP_EOL;
try {
    var_dump( fgetcsv($file_handle, -10, escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( fgetcsv($file_handle, -10, $delimiter, escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump( fgetcsv($file_handle, -10, $delimiter, $enclosure, escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo 'fgetcsv() with delimiter as empty string' . \PHP_EOL;
try {
    var_dump( fgetcsv($file_handle, $length, '', $enclosure, escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo 'fgetcsv() with enclosure as empty string' . \PHP_EOL;
try {
    var_dump( fgetcsv($file_handle, $length, $delimiter, '', escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo 'fgetcsv() with delimiter & enclosure as empty string' . \PHP_EOL;
try {
    var_dump( fgetcsv($file_handle, $length, '', '', escape: "\\") );
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECTF--
fgetcsv() with negative length
ValueError: fgetcsv(): Argument #2 ($length) must be between 0 and %d
ValueError: fgetcsv(): Argument #2 ($length) must be between 0 and %d
ValueError: fgetcsv(): Argument #2 ($length) must be between 0 and %d
fgetcsv() with delimiter as empty string
ValueError: fgetcsv(): Argument #3 ($separator) must be a single character
fgetcsv() with enclosure as empty string
ValueError: fgetcsv(): Argument #4 ($enclosure) must be a single character
fgetcsv() with delimiter & enclosure as empty string
ValueError: fgetcsv(): Argument #3 ($separator) must be a single character
