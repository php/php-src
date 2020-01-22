--TEST--
Bug #78323 Test exit code and error message for invalid parameters
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php
include "include.inc";
$php = get_cgi_path();
reset_env_vars();


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
Error in argument 1, char 1: no argument for option -
Usage: %s
Done: 1

PHP %s
Done: 0
