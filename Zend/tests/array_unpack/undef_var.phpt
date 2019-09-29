--TEST--
array unpacking with undefinded variable
--FILE--
<?php

var_dump([...$arr]);

--EXPECTF--
Notice: Undefined variable: arr in %s on line %d

Fatal error: Uncaught Error: Only arrays and Traversables can be unpacked in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d