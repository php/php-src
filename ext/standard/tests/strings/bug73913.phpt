--TEST--
Bug #73913 broken strrpos with negative offset when needle length longer than offset
--FILE--
<?php

var_dump(strrpos("aaaa", 'aa', -2)); //ER: 0, AR: 2
var_dump(strrpos("aaaa", 'a', -1)); //ER: 2, AR: 3
var_dump(strrpos("/documents/show/5474", '/', -20)); //ER: false, AR: 0
var_dump(strrpos("works_like_a_charm", 'charm', -3)); //ER: false, AR: 13
var_dump(strrpos("works_like_a_charm", 'charm', -4)); //ER: false, AR: 13

var_dump(strripos("AAAA", 'aa', -2)); //ER: 0, AR: 2
var_dump(strripos("AAAA", 'a', -1)); //ER: 2, AR: 3
var_dump(strripos("/documents/show/5474", '/', -20)); //ER: false, AR: 0
var_dump(strripos("WORKS_LIKE_A_CHARM", 'charm', -3)); //ER: false, AR: 13
var_dump(strripos("WORKS_LIKE_A_CHARM", 'charm', -4)); //ER: false, AR: 13

echo "Done\n";
?>
--EXPECTF--	
int(0)
int(2)
bool(false)
bool(false)
bool(false)
int(0)
int(2)
bool(false)
bool(false)
bool(false)
Done
