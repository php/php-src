--TEST--
phpcredits() CGI
--POST--
dummy=x
--FILE--
<?php

var_dump(phpcredits());

echo "--\n";
var_dump(phpcredits(0));

echo "--\n";
var_dump(phpcredits(CREDITS_GROUP));

?>
--EXPECTF--
<!DOCTYPE %a>%s</html>
bool(true)
--
<h1>PHP Credits</h1>
bool(true)
--
<h1>PHP Credits</h1>
%aPHP Group%a
bool(true)
