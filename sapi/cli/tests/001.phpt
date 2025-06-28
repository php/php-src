--TEST--
version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

var_dump(shell_exec("$php -n -v"));

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (cli) (built: %s)%s
Copyright © The PHP Group and Contributors
%AZend Engine v%s, Copyright © Zend by Perforce%A"
Done
