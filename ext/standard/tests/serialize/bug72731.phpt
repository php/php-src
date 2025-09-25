--TEST--
Bug #72731: Type Confusion in Object Deserialization
--FILE--
<?php

class obj {
    var $ryat;
    function __wakeup() {
        $this->ryat = 0x1122334455;
    }
}

$poc = 'O:8:"stdClass":1:{i:0;O:3:"obj":1:{s:4:"ryat";R:1;}}';
var_dump(unserialize($poc));

?>
--EXPECTF--
Deprecated: The __wakeup() serialization magic method has been deprecated. Implement __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
%s(73588229205)
