--TEST--
JIT ROPE: 002 type guards are only checked for strings
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
	$a = 0;
    for ($cnt = 0; $cnt < 6; $cnt++) {
        $array[] = &$y;
        $S["$array[$a] $y"] = $a += $y;
     }
}
foo();
?>
DONE
--EXPECT--
DONE
