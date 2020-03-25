--TEST--
Bug #78263: Handling of self-referential array special case
--FILE--
<?php

// The case of a directly self-referential array is special and will
// be treated as a proper reference despite rc=1 during array copying.
$a = [&$a];
$b = [$a];
unset($a);

var_dump(ReflectionReference::fromArrayElement($b[0], 0));

?>
--EXPECT--
object(ReflectionReference)#1 (0) {
}
