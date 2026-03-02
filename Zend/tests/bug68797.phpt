--TEST--
Bug #68797 Number 2.2250738585072012e-308 converted incorrectly
--INI--
precision=17
--FILE--
<?php

echo 2.2250738585072012e-308, "\n";
?>
--EXPECT--
2.2250738585072014E-308
