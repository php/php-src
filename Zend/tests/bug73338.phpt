--TEST--
Bug #73338: Ensure exceptions in function init opcodes are cleaned properly
--FILE--
<?php

try { call_user_func(new class { function __destruct () { throw new Error; } }); } catch (Error $e) {}

set_error_handler(function() { throw new Error; });

try { var_dump(new stdClass, call_user_func("fail")); } catch (Error $e) {}

try { (function() { call_user_func("fail"); })(); } catch (Error $e) {}

try { [new class { static function foo() {} function __destruct () { throw new Error; } }, "foo"](); } catch (Error $e) {}

?>
--EXPECTF--
Warning: call_user_func() expects parameter 1 to be a valid callback, no array or string given in %s on line %d
