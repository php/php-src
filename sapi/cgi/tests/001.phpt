--TEST--
version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_cgi_path();
reset_env_vars();

var_dump(`$php -n -v`);

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (cgi%s (built: %s
Copyright (c) 1997-20%s The PHP Group
Zend Engine v%s, Copyright (c) 1998-20%s Zend Technologies
"
Done
