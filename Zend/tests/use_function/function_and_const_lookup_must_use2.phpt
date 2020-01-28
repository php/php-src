--TEST--
'function_and_const_lookup='global'' must use correct function name before declaring it
--INI--
error_reporting=E_ALL
--FILE--
<?php
declare(function_and_const_lookup='global');
namespace Other\NS;
use function Another\NS\some_function;
function some_function() {}
--EXPECTF--
Fatal error: Cannot declare function Other\NS\some_function because the name is already in use in %s on line 5
