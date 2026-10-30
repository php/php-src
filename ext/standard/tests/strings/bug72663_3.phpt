--TEST--
Bug #72663: Create an Unexpected Object and Don't Invoke __wakeup() in Deserialization
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
Warning: unserialize(): Error at offset 50 of 50 bytes in %s on line %d
DONE
