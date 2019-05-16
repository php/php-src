--TEST--
Test whether strstr() and strrchr() are binary safe.
--FILE--
<?php
$s = "alabala nica".chr(0)."turska panica";
var_dump(strstr($s, "nic"));
var_dump(strrchr($s," nic"));
?>
--EXPECTREGEX--
string\(18\) \"nica\x00turska panica\"
string\(7\) \" panica\"
