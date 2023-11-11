--TEST--
using invalid combinations of cmdline options
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(`$php -n -a -f "wrong"`);
var_dump(`$php -n -f "wrong" -a`);

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
