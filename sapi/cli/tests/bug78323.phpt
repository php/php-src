--TEST--
Bug #78323 Test exit code and error message for invalid parameters
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

// There are 3 types of option errors:
// 1 : in flags
// 2 option not found
// 3 no argument for option


// colon in flags
ob_start();
passthru("$php -a:Z 2>&1", $exitCode);
$output = ob_get_contents();
ob_end_clean();

$lines = preg_split('/\R/', $output);
echo $lines[0], "\n",
     $lines[1], "\n",
     "Done: $exitCode\n\n";


// option not found
ob_start();
passthru("$php -Z 2>&1", $exitCode);
$output = ob_get_contents();
ob_end_clean();

$lines = preg_split('/\R/', $output);
echo $lines[0], "\n",
     $lines[1], "\n",
     "Done: $exitCode\n\n";


// no argument for option
ob_start();
passthru("$php --memory-limit=1G 2>&1", $exitCode);
$output = ob_get_contents();
ob_end_clean();

$lines = preg_split('/\R/', $output);
echo $lines[0], "\n",
     $lines[1], "\n",
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
Error in argument %d, char %d: : in flags
Usage: %s [options] [-f] <file> [--] [args...]
Done: 1

Error in argument %d, char %d: option not found %s
Usage: %s [options] [-f] <file> [--] [args...]
Done: 1

Error in argument %d, char %d: no argument for option %s
Usage: %s [options] [-f] <file> [--] [args...]
Done: 1

PHP %s
Done: 0
