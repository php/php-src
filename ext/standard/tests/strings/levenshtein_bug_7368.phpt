--TEST--
levenshtein() bug 7368
--FILE--
<?php

var_dump(levenshtein('13458', '12345'));
var_dump(levenshtein('1345', '1234'));

?>
--EXPECT--
int(2)
int(2)
