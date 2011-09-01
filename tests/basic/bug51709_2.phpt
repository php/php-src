--TEST--
Bug #51709 (Can't use keywords as method names)
--FILE--
<?php

class foo {
        static function goto() {
                echo "1";
        }
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Parse error: syntax error, unexpected 'goto' (T_GOTO), expecting identifier (T_STRING) in %sbug51709_2.php on line %d
