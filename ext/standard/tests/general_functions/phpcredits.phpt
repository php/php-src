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
%a

Language Design & Concept
%a

%wPHP %d Authors%w
%a

%wSAPI Modules%w
%a

%wModule Authors%w
%a

%wPHP Documentation%w
%a

PHP Quality Assurance Team
%a

PHP Website Team
%a
bool(true)

Warning: phpcredits() expects parameter 1 to be long, array given in %sphpcredits.php on line 4
NULL
--
PHP Credits
bool(true)
--
PHP Credits

PHP Group
%a
bool(true)
