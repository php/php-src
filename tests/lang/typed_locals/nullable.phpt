--TEST--
Typed local variables: nullable scalar types (?int, ?string)
--FILE--
<?php
?int $a = null;
?int $b = 5;
?string $c = null;
var_dump($a, $b, $c);
?>
--EXPECT--
NULL
int(5)
NULL
