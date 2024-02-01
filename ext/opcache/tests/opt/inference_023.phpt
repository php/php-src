--TEST--
Type inference 023: FETCH_DIM_W
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
function foo($data): array {
    $a = [];
    $n = -1;
    foreach ($data as $d) {
       if ($n >= 0) {
            $a[$n]->x = 2;
       }
       $n++;
       $a[$n] = new stdClass();
       $a[$n]->x = 1;
	}
}
?>
DONE
--EXPECT--
DONE
