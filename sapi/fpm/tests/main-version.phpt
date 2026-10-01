--TEST--
FPM: version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$php = \FPM\Tester::findExecutable();

var_dump(shell_exec("$php -n -v"));

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (fpm%s (built: %s
Copyright © The PHP Group and Contributors
%AZend Engine v%s, Copyright © Zend by Perforce%A"
Done
