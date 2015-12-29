--TEST--
Bug #70644: Trivial Hash complexity DoS (static array)
--FILE--
<?php

$elems = '';
for ($i = 0; $i < 2048; $i++) {
	$elems .= $i * (1<<16) . " => 0, ";
}

$code = <<<PHP
\$x = 0;
return [\$x, $elems];
PHP;

try {
	eval($code);
} catch (HashCollisionError $e) {
	echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Too many collisions in array
