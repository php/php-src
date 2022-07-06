--TEST--
Bug #37176 (iconv_strpos() fails to find a string)
--SKIPIF--
<?php
include('skipif.inc');

$test = @iconv_strpos("abbttt","ttt",0,"UTF-8");
if ($test === false) {
    die("skip UTF-8 is not supported?");
}

?>
--FILE--
<?php
var_dump(iconv_strpos('11--','1-',0,'UTF-8'));
var_dump(iconv_strpos('-11--','1-',0,'UTF-8'));
?>
--EXPECT--
int(1)
int(2)
