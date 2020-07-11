--TEST--
phpcredits()
--FILE--
<?php

var_dump(phpcredits());

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

%wPHP Authors%w
%a

%wSAPI Modules%w
%a

%wModule Authors%w
%a

%wPHP Documentation%w
%a

PHP Quality Assurance Team
%a

%wWebsites and Infrastructure team%w
%a
bool(true)
--
PHP Credits
bool(true)
--
PHP Credits

PHP Group
%a
bool(true)
