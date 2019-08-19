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
$zip = zip_open("");

echo "Test case 2:\n";
$zip = zip_open("/non_exisitng_directory/test_procedural.zip");
echo is_resource($zip) ? "OK" : "Failure";
?>
--EXPECTF--
Test case 1:
Warning: zip_open(): Empty string as source in %s on line %d
Test case 2:
Failure
