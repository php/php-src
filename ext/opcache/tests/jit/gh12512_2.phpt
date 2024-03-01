--TEST--
GH-12512: missing type store
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
function foo(array $a, $exit) {
	$n = 0;

    $count = count($a);
    if ($count == 0) {
        return 0;
    }
    $a2 = [];
    foreach ($a as $v) {
        $a2[] = $v;
    }

    $count = $a2[5];

    for ($i = 0; $i < $count; $i++) {           
        $x = $a[$i];
        for ($k = $i + 1; $k < $count; $k++) {
            $y = $a[$k];
            $n += $x > $y;
        }
	    if ($exit) {
	    	return $n;
	    }
    }

    return $n;
}
var_dump(foo([1,2,3,4,5,6,7,8], 1));
var_dump(foo([1,2,3,4,5,6,7,8], 1));
var_dump(foo([1,2,3,4,5,6,7,8], 0));
?>
DONE
--EXPECT--
int(0)
int(0)
int(0)
DONE
