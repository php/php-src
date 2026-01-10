--TEST--
JIT FETCH_DIM_R: 013
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function foo() {
	$y = 0; $tokens = [];
    for($cnt = 0; $cnt < 6; $cnt++) {
        $tokens[$y] > $tokens[$y][] = $y;
     }
}
@foo();
?>
DONE
--EXPECT--
DONE
