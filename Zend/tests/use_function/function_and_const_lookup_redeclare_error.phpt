--TEST--
'function_and_const_lookup='global'' with name reuse
--FILE--
<?php
declare(function_and_const_lookup='global');
namespace Other;

// Preserve the existing fatal error for redeclaring a function that was used by name.
use function printf;

function printf(string $msg, $args) {
    printf("Prefix $msg", ...$args);
}
--EXPECTF--
Warning: The 'use function printf;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 6

Fatal error: Cannot declare function Other\printf because the name is already in use in %s on line 8
