--TEST--
version_compare() rejects NUL bytes in version strings
--FILE--
<?php
foreach ([
    ["1.0\0suffix", '1.0'],
    ['1.0', "1.0\0suffix"],
    ["\0", '1.0'],
    ['1.0', "\0"],
] as [$version1, $version2]) {
    try {
        version_compare($version1, $version2, 'eq');
    } catch (ValueError $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
var_dump(version_compare('1.0', '1.0'));
var_dump(version_compare('1.0', '1.0', 'eq'));
?>
--EXPECT--
ValueError: version_compare(): Argument #1 ($version1) must not contain any null bytes
ValueError: version_compare(): Argument #2 ($version2) must not contain any null bytes
ValueError: version_compare(): Argument #1 ($version1) must not contain any null bytes
ValueError: version_compare(): Argument #2 ($version2) must not contain any null bytes
int(0)
bool(true)
