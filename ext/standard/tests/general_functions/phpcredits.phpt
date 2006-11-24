--TEST--
phpcredits()
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
PHP Credits

PHP Group
%s

Language Design & Concept
%s

%wPHP %d Authors%w
%s

%wSAPI Modules%w
%s

%wModule Authors%w
%s

%wPHP Documentation%w
%s

PHP Quality Assurance Team
%s

PHP Website Team
%s
bool(true)

Warning: phpcredits() expects parameter 1 to be long, array given in %sphpcredits.php on line 4
NULL
--
PHP Credits
bool(true)
--
PHP Credits

PHP Group
%s
bool(true)
