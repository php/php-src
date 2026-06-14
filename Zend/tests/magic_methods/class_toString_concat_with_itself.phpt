--TEST--
Test concatenating a class instance that has __toString with itself
--FILE--
<?php
class Tmp {
	public function __toString() {
		return "abc";
	}
}

$tmp = new Tmp;
$tmp .= $tmp;
echo $tmp . "\n";
?>
--EXPECT--
abcabc
