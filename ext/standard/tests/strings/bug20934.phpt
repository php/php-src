--TEST--
Bug #20934 (htmlspecialchars returns latin1 from UTF-8)
--FILE--
<?php
$str = "\xc3\xa0\xc3\xa1";
echo bin2hex($str), "\n";
echo bin2hex(htmlspecialchars($str, ENT_COMPAT, "UTF-8")), "\n";
?>
--EXPECTF--
c3a0c3a1
c3a0c3a1
