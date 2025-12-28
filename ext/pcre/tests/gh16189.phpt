--TEST--
GH-16189 (preg_match/preg_match_all underflow on start_offset argument)
--FILE--
<?php

try {
	preg_match( '/<(\w+)[\s\w\-]+ id="S44_i89ew">/', '<br><div id="S44_i89ew">', $matches, 0, PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
try {
	preg_match_all( '/<(\w+)[\s\w\-]+ id="S44_i89ew">/', '<br><div id="S44_i89ew">', $matches, 0, PHP_INT_MIN);
} catch (\ValueError $e) {
	echo $e->getMessage() . PHP_EOL;
}
?>
--EXPECTF--
preg_match(): Argument #5 ($offset) must be greater than %s
preg_match_all(): Argument #5 ($offset) must be greater than %s
