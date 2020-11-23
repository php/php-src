--TEST--
iconv() test 3
--SKIPIF--
<?php extension_loaded('iconv') or die('skip iconv extension is not available'); ?>
--FILE--
<?php
for ($i = 0; $i < 3; ++$i) {
    if (@iconv('blah', 'blah', 'blah') != '') {
        die("failed\n");
    }
}
echo "success\n";
?>
--EXPECT--
success
