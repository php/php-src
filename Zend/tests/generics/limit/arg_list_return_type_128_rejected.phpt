--TEST--
Generics: 127 type arguments on a function return type is OK; 128 is a compile error
--FILE--
<?php

$N = implode(",", array_fill(0, 127, "int"));
eval("function f127(): Box<{$N}> {}");
echo "OK\n";

$N = implode(",", array_fill(0, 128, "int"));
eval("function f128(): Box<{$N}> {}");
?>
--EXPECTF--
OK

Fatal error: Cannot specify more than 127 generic type arguments (got 128) in %s : eval()'d code on line %d
