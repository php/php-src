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
Warning: Directive 'magic_quotes_gpc' is deprecated in PHP 5.3 and greater in Unknown on line 0
\"probably a bug\"
\"probably a bug\"