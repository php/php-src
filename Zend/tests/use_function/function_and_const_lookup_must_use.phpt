--TEST--
'function_and_const_lookup='global'' must use function name before declaring it
--INI--
error_reporting=E_ALL
--FILE--
<?php
declare(function_and_const_lookup='global');
namespace Other\NS;
function some_function() {}
--EXPECTF--
Fatal error: Cannot declare function Other\NS\some_function because the name is already in use due to declare(function_and_const_lookup='global'). Add 'use function Other\NS\some_function;' above to fix that in %s on line 4
