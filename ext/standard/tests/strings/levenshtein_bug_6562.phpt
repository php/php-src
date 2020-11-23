--TEST--
levenshtein() bug 6562
--FILE--
<?php

var_dump(levenshtein('debugg', 'debug'));
var_dump(levenshtein('ddebug', 'debug'));
var_dump(levenshtein('debbbug', 'debug'));
var_dump(levenshtein('debugging', 'debuging'));

?>
--EXPECT--
int(1)
int(1)
int(2)
int(1)
