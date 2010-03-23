--TEST--
unset() CV 6 (indirect unset() of global variable in session_unset())
--SKIPIF--
<?php include(dirname(__FILE__).'/../../ext/session/tests/skipif.inc'); ?>
--INI--
register_globals=1
session.auto_start=0
session.save_handler=files
--FILE--
<?php
$x = "1\n";
session_start();
echo $x;
session_register('x');
$_SESSION['x'] = "2\n";
echo $x;
session_unset();
echo $x;
echo "ok\n";
?>
--EXPECTF--
Warning: Directive 'register_globals' is deprecated in PHP %d.%d and greater in Unknown on line 0
1

Deprecated: Function session_register() is deprecated in %s on line %d
2

Notice: Undefined variable: x in %sunset_cv06.php on line %d
ok
