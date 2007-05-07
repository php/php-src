--TEST--
Bug #39763 (filter applies magic_quotes twice in parse_str())
--INI--
magic_quotes_gpc=1
filter.default=
--FILE--
<?php
$arr = array();
parse_str("val=%22probably+a+bug%22", $arr);
echo $arr['val'] . "\n";
parse_str("val=%22probably+a+bug%22");
echo $val . "\n";
?>
--EXPECT--	
\"probably a bug\"
\"probably a bug\"
