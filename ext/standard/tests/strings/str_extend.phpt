--TEST--
Append to string allocated with str_extend()
--FILE--
<?php

$str = str_extend("a", 1 << 22);
for ($i = 0; $i < 1 << 21; ++$i) {
	$str .= "a";
}

var_dump(array_filter(count_chars($str)));

?>
--EXPECT--
array(1) {
  [97]=>
  int(2097153)
}