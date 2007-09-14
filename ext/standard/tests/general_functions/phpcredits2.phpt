--TEST--
phpcredits() CGI
--SKIPIF--
<?php if (php_sapi_name()=='cli') echo 'skip'; ?>
--POST--
dummy=x
--FILE--
<?php

var_dump(phpcredits());
var_dump(phpcredits(array()));

echo "--\n";
var_dump(phpcredits(0));

echo "--\n";
var_dump(phpcredits(CREDITS_GROUP));

?>
--EXPECTF--
<!DOCTYPE %a>%s</html>
bool(true)

Warning: phpcredits() expects parameter 1 to be long, array given in %sphpcredits2.php on line 4
NULL
--
<h1>PHP Credits</h1>
bool(true)
--
<h1>PHP Credits</h1>
%aPHP Group%a
bool(true)
