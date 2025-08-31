--TEST--
iconv() test 3
--EXTENSIONS--
iconv
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
