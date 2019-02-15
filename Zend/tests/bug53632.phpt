--TEST--
zend_strtod() hangs with 2.2250738585072011e-308
--FILE--
<?php
$d = 2.2250738585072011e-308;

echo "Done\n";
?>
--EXPECT--
Done
