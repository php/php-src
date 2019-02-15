--TEST--
version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n -v`);

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (cli) (built: %s)%s
Copyright (c) The PHP Group
Zend Engine v%s, Copyright (c) Zend Technologies
"
Done
