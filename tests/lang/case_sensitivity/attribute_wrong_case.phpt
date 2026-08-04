--TEST--
Internal attribute recognition and getAttributes() filtering are case-sensitive
--FILE--
<?php
#[Deprecated(message: "use bar() instead")]
function foo() {}

#[deprecated]
function bar() {}

// Exact case is recognized as the internal attribute.
foo();

// Wrong case is not recognized: no deprecation is raised.
bar();

// getAttributes() name filter is case-sensitive.
$r = new ReflectionFunction('foo');
var_dump(count($r->getAttributes('Deprecated')));
var_dump(count($r->getAttributes('DEPRECATED')));

$r2 = new ReflectionFunction('bar');
var_dump($r2->getAttributes()[0]->getName());
?>
--EXPECTF--
Deprecated: Function foo() is deprecated, use bar() instead in %s on line %d
int(1)
int(0)
string(10) "deprecated"
