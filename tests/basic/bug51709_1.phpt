--TEST--
Bug #51709 (Can't use keywords as method names)
--FILE--
<?php

class foo {
        static function for() {
                echo "1";
        }
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Parse error: syntax error, unexpected "for (T_FOR)", expecting "identifier (T_STRING)" in %sbug51709_1.php on line %d
