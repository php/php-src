--TEST--
version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(shell_exec("$php -n -v"));

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (cgi%s (built: %s
Copyright © The PHP Group and Contributors
%AZend Engine v%s, Copyright © Zend by Perforce%A"
Done
