--TEST--
Bug #73367: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization
--FILE--
<?php

class obj {
	var $ryat;
	function __wakeup() {
		$this->ryat = null;
        echo "wakeup\n";
		throw new Exception("Not a serializable object");
	}
	function __destruct() {
        echo "dtor\n";
		if ($this->ryat == 1) {
            echo "dtor ryat==1\n";
		}
	}
}

$poc = 'O:3:"obj":2:{s:4:"ryat";i:1;i:0;O:3:"obj":1:{s:4:"ryat";R:1;}}';
try {
    unserialize($poc);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
wakeup
Not a serializable object
