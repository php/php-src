--TEST--
Bug #34087 (strtotime() does not work with date format "Y/m/d")
--FILE--
<?php
echo "Y/m/d: ", strtotime("2005/8/12"), "\n";
echo "Y-m-d: ", strtotime("2005-8-12");
?>
--EXPECT--
Y/m/d: 1123804800
Y-m-d: 1123804800
