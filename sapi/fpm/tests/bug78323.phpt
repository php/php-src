--TEST--
FPM: Bug #78323 Test exit code for invalid parameters
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

require_once "tester.inc";

$php = \FPM\Tester::findExecutable();

// no argument for option
ob_start();
passthru("$php --memory-limit=1G 2>&1", $exitCode);
$output = ob_get_contents();
ob_end_clean();

$lines = preg_split('/\R/', $output);
echo $lines[0], "\n",
     "Done: $exitCode\n\n";


// Successful execution
ob_start();
passthru("$php -dmemory-limit=1G -v", $exitCode);
$output = ob_get_contents();
ob_end_clean();

$lines = preg_split('/\R/', $output);
echo $lines[0], "\n",
     "Done: $exitCode\n";

?>
--EXPECTF--
Usage: %s
Done: 64

PHP %s
Done: 0
