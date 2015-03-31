--TEST--
Bug #69335 (SplArray BC break)
--XFAIL--
see https://bugs.php.net/bug.php?id=69335
--FILE--
<?php
$a = array(1=>1, 3=>3, 5=>5, 7=>7);
$a = new ArrayObject($a);

foreach ($a as $k => $v) {
	var_dump("$k => $v");
	if ($k == 3) {
		$a['a'] = "?";
	}
}
?>
--EXPECTF--
string(6) "1 => 1"
string(6) "3 => 3"
string(6) "5 => 5"
string(6) "7 => 7"
string(6) "a => ?"
