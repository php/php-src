--TEST--
Bug #77428: mb_ereg_replace() doesn't replace a substitution variable 
--FILE--
<?php

// This behavior is broken, but kept for BC reasons
var_dump(mb_ereg_replace('(%)', '\\\1', 'a%c'));
// For clarify, the above line is equivalent to:
var_dump(mb_ereg_replace('(%)', '\\\\1', 'a%c'));

?>
--EXPECT--
string(4) "a\%c"
string(4) "a\%c"
