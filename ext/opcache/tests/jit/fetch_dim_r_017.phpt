--TEST--
JIT FETCH_DIM_R: 017
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 0; $i < 20; $i++) {
		$obj->prop0 = $a =! --$a > $a =! --$a + $a = ($array[$a]);
        $obj->prop0 = $a =! --$a > $a =! --$a + $a = ($array[$a]);
        $array = array(312 > 0);
        $a = ($array[$a]);
    }
}
try {
	@test();
} catch (Throwable $ex) {
}
?>
DONE
--EXPECT--
DONE
