--TEST--
Test whether strstr() and strrchr() are binary safe.
--FILE--
<?php
/* Do not change this test it is a README.TESTING example. */
$s = "alabala nica".chr(0)."turska panica";
var_dump(strstr($s, "nic"));
var_dump(strrchr($s," nic"));
?>
--EXPECTREGEX--
unicode\(18\) \"nica\x00turska panica\"
unicode\(7\) \" panica\"
