--TEST--
Test extract() function (variation 8)
--FILE--
<?php

/* To show variables with numerical prefixes cannot be extracted */
$var["i"] = 1;
$var["j"] = 2;
$var["k"] = 3;
echo "\n*** Testing for Numerical prefixes ***\n";
var_dump(extract($var));

$var1["m"] = 1;
$var1[2] = 2;
$var1[] = 3;
var_dump ( extract($var1));

echo "\nDone";
?>
--EXPECTF--
*** Testing for Numerical prefixes ***
int(3)
int(1)

Done
