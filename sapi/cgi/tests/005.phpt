--TEST--
using invalid combinations of cmdline options
--SKIPIF--
<?php

if (substr(PHP_OS, 0, 3) == 'WIN') {
	die ("skip not for Windows");
}

include "skipif.inc";
?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(`$php -n -a -f 'wrong'`);
var_dump(`$php -n -f 'wrong' -a`);

echo "Done\n";
?>
--EXPECTF--	
string(51) "No input file specified.
Interactive mode enabled

"
string(51) "No input file specified.
Interactive mode enabled

"
Done
