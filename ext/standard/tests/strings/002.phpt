--TEST--
Test whether strstr() and strrchr() are binary safe.
--POST--
--GET--
--FILE--
<?php
$s = "alabala nica".chr(0)."turska panica";
var_dump(strlen(strstr($s, "nic")));
var_dump(strlen(strrchr($s," nic")));
?>
--EXPECT--
int(18)
int(19)