--TEST--
ReflectionTypes of relative class types (self, parent) in Closure bound to class with parent
--FILE--
<?php

class A {}
class B {}

$f = function (): self {
    return new self();
};

$f = Closure::bind($f, new A, B::class);
$r = new ReflectionFunction($f);
$resolved = $r->getReturnType()->resolveToNamedType()->__toString();

// Prints B == A
printf("%s == %s ?\n", $f()::class, $resolved);

?>
--EXPECT--
B == B ?
