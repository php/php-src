--TEST--
Register Alloction 016: Missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
    for($cnt = 0; $cnt < 6; $cnt++) {
    	var_dump($x);
        $a - 536 >> 4 - $y - 4 << ++$x == $a ?: $b;
        $a .= !$a;
        $x = $a ? $b : $b;
    }
}
@foo();
?>
DONE
--EXPECTF--
NULL
NULL
NULL
NULL
NULL
NULL
DONE
