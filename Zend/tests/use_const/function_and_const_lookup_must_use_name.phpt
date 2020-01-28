--TEST--
function_and_const_lookup="global" must use const name before declaration
--FILE--
<?php
declare(function_and_const_lookup="global");
namespace Other {
// It is an error to declare MY_CONSTANT with a conflicting namespaced name.
// To work around this, do one of the following:
// 1. Add 'use const Other\MY_CONSTANT;' above to make it clear which name is being used.
// 2. Use declare(function_and_const_lookup="default") or no declare.
// 3. Use define()
const MY_CONSTANT = 'MY_CONSTANT(NAMESPACED)';
echo MY_CONSTANT;
}
--EXPECTF--
Fatal error: Cannot declare const Other\MY_CONSTANT because the name is already in use due to declare(function_and_const_lookup='global'). Add 'use const Other\MY_CONSTANT;' above to fix that in %s on line 9
