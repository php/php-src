--TEST--
Bug #46781 (BC math handles minus zero incorrectly)
--EXTENSIONS--
bcmath
--FILE--
<?php
var_dump(bcadd('-0.0', '-0.0', 1));
var_dump(bccomp('-0.0', '0', 1));
?>
--EXPECT--
string(3) "0.0"
int(0)
