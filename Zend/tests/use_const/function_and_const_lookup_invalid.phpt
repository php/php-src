--TEST--
function_and_const_lookup=<invalid>
--FILE--
<?php
// This does not accept quotes.
declare(function_and_const_lookup='invalid');
--EXPECTF--
Fatal error: declare(function_and_const_lookup) value must be the string 'global' or 'default' in %s on line 3
