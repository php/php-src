--TEST--
FPM: Bug #78323 Test exit code for invalid parameters
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$php = \FPM\Tester::findExecutable();

passthru("$php --foo-bar", $exitCode);

echo "\nDone: $exitCode";
?>
--EXPECTF--
Usage: %s
   %a

Done: 1
