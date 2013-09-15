--TEST--
Bug #51709 (Use keywords as method or class names)
--FILE--
<?php

class List {
        static function goto() {
                echo "OK";
        }
}

List::goto();

?>
--EXPECTF--
OK
