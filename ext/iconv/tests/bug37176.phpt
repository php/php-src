--TEST--
Bug #37176 (iconv_strpos() fails to find a string)
--FILE--
<?php
var_dump(iconv_strpos('11--','1-',0,'UTF-8'));
var_dump(iconv_strpos('-11--','1-',0,'UTF-8'));
?>
--EXPECT--
int(1)
int(2)
