--TEST--
Bug #73367: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization (2)
--FILE--
<?php

class obj {
    var $ryat;
    function __wakeup() {
        $this->ryat = null;
        $this->foo = 'bar'; // $this should still be live here
        echo "wakeup\n";
    }
}

$poc = 'O:3:"obj":2:{s:4:"ryat";i:1;i:0;O:3:"obj":1:{s:4:"ryat";R:1;}}';
unserialize($poc);

?>
--EXPECT--
wakeup
wakeup
