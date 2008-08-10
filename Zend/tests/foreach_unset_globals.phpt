--TEST--
traverse an array and use its keys to unset GLOBALS
--FILE--
<?php

$arr = array("a" => 1, "b" => 2);
foreach ($arr as $key => $val) {
	unset($GLOBALS[$key]);
}

var_dump($arr);
echo "Done\n";
?>
--EXPECTF--	
array(2) {
  [u"a"]=>
  int(1)
  [u"b"]=>
  int(2)
}
Done
