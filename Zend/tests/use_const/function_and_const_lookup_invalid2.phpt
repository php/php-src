--TEST--
'strict_types=<invalid>'
--FILE--
<?php
// This is an invalid value that zend_compile.c should give a proper error message for.
// The value `global` only makes sense for function_and_const_lookup.
declare(strict_types=global);
--EXPECTF--
Fatal error: declare(strict_types) value must be a literal in %s on line 4
