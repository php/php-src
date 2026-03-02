--TEST--
GH-16499 (Undefined to null coercion issues for return)
--EXTENSIONS--
opcache
--INI--
opcache.jit_buffer_size=64M
--FILE--
<?php
function test($cond): ?int {
    if ($cond) {
        $i = 'foo';
    }
    return $i;
}

var_dump(test(false));
var_dump(test(false));
?>
--EXPECTF--
Warning: Undefined variable $i in %sgh16499.php on line 6

Warning: Undefined variable $i in %sgh16499.php on line 6
NULL

Warning: Undefined variable $i in %sgh16499.php on line 6

Warning: Undefined variable $i in %sgh16499.php on line 6
NULL