--TEST--
FPM: version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$php = \FPM\Tester::findExecutable();

var_dump(`$php -n -v`);

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (fpm%s (built: %s
Copyright (c) The PHP Group
Zend Engine v%s, Copyright (c) Zend Technologies
"
Done
