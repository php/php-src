--TEST--
Bug #79979 (passing value to by-ref param via CUF(A) crashes)
--FILE--
<?php

namespace Foo;

call_user_func_array("str_replace", ["a", "b", "c", new \stdClass]);
call_user_func_array("str_replace", ["a", "b", "c", "replace_count" => new \stdClass]);

\call_user_func_array("str_replace", ["a", "b", "c", new \stdClass]);
\call_user_func_array("str_replace", ["a", "b", "c", "replace_count" => new \stdClass]);

?>
--EXPECTF--
Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d

Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d

Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d

Warning: str_replace(): Argument #4 ($replace_count) must be passed by reference, value given in %s on line %d
