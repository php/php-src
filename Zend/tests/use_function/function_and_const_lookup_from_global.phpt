--TEST--
'function_and_const_lookup='global'' with redundant uses.
--FILE--
<?php
// It's pointless in the global namespace.
declare(function_and_const_lookup='global');
// Preserve the existing fatal error for redeclaring a function that was used?

use function intdiv;
use function other\func;  // should not warn
use function printf;
use function VAR_export as var_EXPORT;
printf("5/2 = %d\n", intdiv(5,2));
--EXPECTF--
Warning: The use statement with non-compound name 'intdiv' has no effect in %s on line 6

Warning: The 'use function intdiv;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 6

Warning: The use statement with non-compound name 'printf' has no effect in %s on line 8

Warning: The 'use function printf;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 8

Warning: The 'use function VAR_export;' is redundant due to 'declare(function_and_const_lookup="global")' in %s on line 9
5/2 = 2