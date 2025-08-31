--TEST--
levenshtein() bug 16473
--FILE--
<?php

var_dump(levenshtein('a', 'bc'));
var_dump(levenshtein('xa', 'xbc'));
var_dump(levenshtein('xax', 'xbcx'));
var_dump(levenshtein('ax', 'bcx'));

?>
--EXPECT--
int(2)
int(2)
int(2)
int(2)
