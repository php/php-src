--TEST--
Reflection: ReflectionGenericVariance is a unit enum exposing three cases
--FILE--
<?php
echo ReflectionGenericVariance::Invariant->name, "\n";
echo ReflectionGenericVariance::Covariant->name, "\n";
echo ReflectionGenericVariance::Contravariant->name, "\n";
var_dump(ReflectionGenericVariance::Invariant instanceof BackedEnum);
var_dump(ReflectionGenericVariance::Invariant === ReflectionGenericVariance::Invariant);
var_dump(ReflectionGenericVariance::Invariant === ReflectionGenericVariance::Covariant);
?>
--EXPECT--
Invariant
Covariant
Contravariant
bool(false)
bool(true)
bool(false)
