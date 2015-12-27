--TEST--
Enum comparisons
--FILE--
<?php

enum foo { a, b }
enum bar { a, c }

print "Equality of enum\n";
var_dump(foo::a == foo::a);
var_dump(foo::a === foo::a);
var_dump(bar::a == foo::a);
var_dump(bar::a === foo::a);
var_dump(foo::a == foo::b);
var_dump(foo::a === foo::b);

print "Ternary order of enum\n";
var_dump((foo::a <=> 1) === - (1 <=> foo::a)); // order between types is undefined, but guaranteed to be symmetric
var_dump((foo::a <=> foo::b) === (foo::b <=> foo::a)); // order between enums is unordered

print "Equality to other types\n";
var_dump((bool) foo::a);
var_dump(foo::a == true);
var_dump(foo::a === true);
var_dump(foo::a == false);
var_dump(foo::a == null);
var_dump(foo::a ? 1 : 0);

?>
--EXPECT--
Equality of enum
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
Ternary order of enum
bool(true)
bool(true)
Equality to other types
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
int(1)
