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
Parse error: syntax error, unexpected T_GOTO, expecting T_STRING in %s/bug51709_2.php on line %d
