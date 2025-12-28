--TEST--
Test concatenating a class instance that has __toString with itself that uses a non-interned string
--FILE--
<?php
$global_non_interned_string = str_repeat("a", 3);

class Test {
	public function __toString() {
		global $global_non_interned_string;
		return $global_non_interned_string;
	}
}

$test1 = new Test;
$test2 = new Test;
$test1 .= $test2;

echo $test1 . "\n";
?>
--EXPECT--
aaaaaa
