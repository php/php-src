--TEST--
array_map(): unresolved PFA callback uses native fallback
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--ENV--
A=1
--FILE--
<?php

if (getenv('A')) {
    function plusn($x, $n) {
        return $x + $n;
    }
}

var_dump(array_map(plusn(?, 2), range(1, 2)));

?>
--EXPECT--
array(2) {
  [0]=>
  int(3)
  [1]=>
  int(4)
}
