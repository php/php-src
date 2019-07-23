--TEST--
Bug #78323 Test exit code for invalid parameters
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

passthru("$php --memory-limit=1G", $exitCode);

echo "Done: $exitCode\n\n";

passthru("$php -dmemory-limit=1G -v", $exitCode);

echo "\nDone: $exitCode";
?>
--EXPECTF--

Usage: %s [options] [-f] <file> [--] [args...]
   %a

Done: 1

PHP %s
%a

Done: 0

