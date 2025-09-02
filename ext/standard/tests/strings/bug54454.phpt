--TEST--
Bug #54454 (substr_compare incorrectly reports equality in some cases)
--FILE--
<?php
var_dump(substr_compare('/', '/asd', 0, 4));
?>
--EXPECT--
int(-1)
