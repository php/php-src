--TEST--
FPM: version string
--SKIPIF--
<?php include "skipif.inc"; ?>
--FILE--
<?php

include "include.inc";

$php = get_fpm_path();

var_dump(`$php -n -v`);

echo "Done\n";
?>
--EXPECTF--
string(%d) "PHP %s (fpm%s (built: %s
Copyright (c) 1997-20%s The PHP Group
Zend Engine v%s, Copyright (c) 1998-20%s Zend Technologies
"
Done
