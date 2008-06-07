--TEST--
SPL: FastArray: Assigning the itself object
--FILE--
<?php

$b = 10;
$a = new SplFastArray($b);

try {
	$a[1] = $a;
} catch (Exception $e) {
	echo $e->getMessage(), "\n";
}

foreach ($a as $c) {
	if ($c) {
		echo $c->getSize(), "\n";
	}
}

print "ok\n";

?>
--EXPECT--
10
ok
