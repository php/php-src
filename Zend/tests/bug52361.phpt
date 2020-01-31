--TEST--
Bug #52361 (Throwing an exception in a destructor causes invalid catching)
--FILE--
<?php
class aaa {
	public function __destruct() {
		try {
			throw new Exception(__CLASS__);
		} catch(Exception $ex) {
			echo "1. $ex\n";
		}
	}
}
function bbb() {
	$a = new aaa();
	throw new Exception(__FUNCTION__);
}
try {
	bbb();
	echo "must be skipped !!!";
} catch(Exception $ex) {
	echo "2. $ex\n";
}
?>
--EXPECTF--
1. Exception: aaa in %sbug52361.php:5
Stack trace:
#0 %sbug52361.php(16): aaa->__destruct()
#1 {main}
2. Exception: bbb in %sbug52361.php:13
Stack trace:
#0 %sbug52361.php(16): bbb()
#1 {main}
