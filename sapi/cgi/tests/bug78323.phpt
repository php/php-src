--TEST--
Bug #78323 Test exit code for invalid parameters
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

passthru("$php --foo-bar", $exitCode);

echo "\nDone: $exitCode";
?>
--EXPECTF--
Error in argument 1, char 1: no argument for option -
Usage: %s
    %a

Done 1
