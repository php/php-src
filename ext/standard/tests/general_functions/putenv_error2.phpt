--TEST--
putenv() - usage in safe mode with safe_mode_protected_env_vars set
--CREDITS--
Brian DeShong <brian@deshong.net>
--INI--
safe_mode=1
safe_mode_protected_env_vars=FOO,BAZ
--FILE--
<?php
putenv('FOO=bar');
putenv('BAZ=bop');
?>
==DONE==
--EXPECTF--
Warning: Directive 'safe_mode' is deprecated in PHP 5.3 and greater in Unknown on line 0

Warning: putenv(): Safe Mode warning: Cannot override protected environment variable 'FOO' in %s on line %d

Warning: putenv(): Safe Mode warning: Cannot override protected environment variable 'BAZ' in %s on line %d
==DONE==
