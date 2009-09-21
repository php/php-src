--TEST--
putenv() - usage in safe mode with safe_mode_allowed_env_vars set
--CREDITS--
Brian DeShong <brian@deshong.net>
--INI--
safe_mode=1
safe_mode_allowed_env_vars=TESTING_
--FILE--
<?php
putenv('FOO=bar');
putenv('BAZ=bop');
$set = putenv('TESTING_FOO=bar');
print ($set ? 'it worked' : 'boo') . "\n";
$set = putenv('TESTING_BAR=baz');
print ($set ? 'it worked' : 'boo') . "\n";
?>
==DONE==
--EXPECTF--
Warning: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in Unknown on line 0

Warning: putenv(): Safe Mode warning: Cannot set environment variable 'FOO' - it's not in the allowed list in %s on line %d

Warning: putenv(): Safe Mode warning: Cannot set environment variable 'BAZ' - it's not in the allowed list in %s on line %d
it worked
it worked
==DONE==
