--TEST--
func_get_args() outside of a function declaration
--FILE--
<?php

var_dump(func_get_args());	

?>
--EXPECTREGEX--
Warning\: func_get_args\(\)\:  Called from the global scope - no function context in \S* on line 3
bool\(false\)
