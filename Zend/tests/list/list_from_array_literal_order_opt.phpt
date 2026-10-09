--TEST--
List assignment from an array literal: evaluation order and lifetime of the values, with the optimizer
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
--FILE--
<?php
require __DIR__ . '/list_from_array_literal_order.inc';
?>
--EXPECT--
-- values before targets
value 1
value 2
key a
key b
-- extra values
construct a
construct x
construct y
destruct x
destruct y
assigned
destruct a
-- values nobody takes live until the assignments are done
construct old x
construct y
destruct y
destruct old x
assigned
-- a value the target does not keep lives until the assignments are done
construct k
construct l
set k
set l
destruct k
destruct l
assigned
construct old b
construct first
construct second
destruct old b
destruct first
assigned
destruct second
-- the same with a destructor that throws
boom
int(42)
-- two targets that are references to each other
boom
int(42)
-- a variable that is a reference to a typed property
construct coerced
set y
destruct coerced
assigned
-- pending values when a target expression throws
construct v1
construct v2
construct v3
destruct v2
destruct v3
destruct v1
boom
-- a variable is copied before any assignment
construct o
construct new o
assigned
string(5) "new o"
string(1) "o"
destruct new o
destruct o
-- exception in a later value
construct first
construct second
destruct first
destruct second
boom
-- exception in an assignment
construct after the throw
destruct after the throw
TypeError
int(1)
-- a destructor that throws during the release after the assignments
construct a
construct b
set a
set b
destruct a throws
destruct b
boom a
done
