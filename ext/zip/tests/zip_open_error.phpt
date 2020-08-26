--TEST--
zip_open() error conditions
--CREDITS--
Birgitte Kvarme <bitta@redpill-linpro.com>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
echo "Test case 1:";
try {
    $zip = zip_open("");
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Test case 2:\n";
$zip = zip_open("/non_exisitng_directory/test_procedural.zip");
echo is_resource($zip) ? "OK" : "Failure";
?>
--EXPECTF--
Test case 1:
Deprecated: Function zip_open() is deprecated in %s on line %d
zip_open(): Argument #1 ($filename) cannot be empty
Test case 2:

Deprecated: Function zip_open() is deprecated in %s on line %d
Failure
