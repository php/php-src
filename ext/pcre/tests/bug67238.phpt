--TEST--
Bug #67238 Ungreedy and min/max quantifier bug in PCRE 8.34 upstream
--FILE--
<?php

echo preg_match('/a{1,3}b/U', 'ab');

?>
--EXPECT--
1
