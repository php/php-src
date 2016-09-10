--TEST--
Bug #72663: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization
--XFAIL--
Memory leak, TBF later.
--FILE--
<?php
class obj {
	var $ryat;
	function __wakeup() {
		$this->ryat = str_repeat('A', 0x112);
	}
}

$poc = 'O:8:"stdClass":1:{i:0;O:3:"obj":1:{s:4:"ryat";R:1;';
unserialize($poc);
?>
DONE
--EXPECTF--
Notice: unserialize(): Error at offset 51 of 50 bytes in %sbug72663_3.php on line %d
DONE
