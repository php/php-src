--TEST--
FE_FETCH 001: SSA reconstruction when body of "foreach" loop is removed
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function test() {
	$j = 0;
    for ($i = 0; $i < 77; $i++) {
        for ($i = 5; $i <= 1; $i++)
            $obj = (object)$arr;
        foreach ($obj as $val)
            if ($j < 14) break;
    }
}
?>
DONE
--EXPECT--
DONE
