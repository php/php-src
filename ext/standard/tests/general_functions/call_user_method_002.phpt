--TEST--
call_user_method() Invalid free
--FILE--
<?php

call_user_method("1", $arr1);

?>
--EXPECTF--
Deprecated: Function call_user_method() is deprecated in %s on line %d

Warning: call_user_method(): Second argument is not an object or class name in %s on line %d
