--TEST--
using invalid combinations of cmdline options
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(shell_exec(<<<SHELL
$php -n -a -f "wrong"
SHELL));
var_dump(shell_exec(<<<SHELL
$php -n -f "wrong" -a
SHELL));

echo "Done\n";
?>
--EXPECT--
string(51) "Interactive mode enabled

No input file specified.
"
string(51) "Interactive mode enabled

No input file specified.
"
Done
