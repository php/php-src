--TEST--
mkdir(): invalid mode
--FILE--
<?php
$testCases = [
    1000000,    // way too large
    -1,         // negative
    0o10000,    // above 0o7777
    0x1FFFF,    // hex, still too large
    12345,      // arbitrary invalid
];

foreach ($testCases as $mode) {
    try {
        echo "Testing mode: $mode\n";
        mkdir(__DIR__ . "/testdir_$mode", $mode);
    } catch (ValueError $e) {
        echo $e->getMessage(), PHP_EOL;
    } catch (Exception $e) {
        echo "Other exception: ", $e->getMessage(), PHP_EOL;
    }
}
?>
--EXPECT--
Testing mode: 1000000
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
Testing mode: -1
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
Testing mode: 4096
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
Testing mode: 131071
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
Testing mode: 12345
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
