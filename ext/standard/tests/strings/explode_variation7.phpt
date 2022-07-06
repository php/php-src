--TEST--
Test explode() function : usage variations - when $string length is greater than
EXPLODE_ALLOC_STEP (64) and the $limit is negative
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php

var_dump(count(explode('|', implode('|', range(1,65)), -1)));

?>
--EXPECT--
int(64)
