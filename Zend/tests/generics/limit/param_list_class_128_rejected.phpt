--TEST--
Generics: 127 type parameters on a class is OK; 128 is a compile error
--FILE--
<?php
$names127 = implode(",", array_map(fn($i) => "T{$i}", range(0, 126)));
eval("class C127<{$names127}> {}");
echo "OK\n";

$names128 = implode(",", array_map(fn($i) => "T{$i}", range(0, 127)));
eval("class C128<{$names128}> {}");
?>
--EXPECTF--
OK

Fatal error: Cannot declare more than 127 generic type parameters (got 128) in %s : eval()'d code on line %d
