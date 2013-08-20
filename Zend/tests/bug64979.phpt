--TEST--
Bug #64578 (Closures with static variables can be generators)
--XFAIL--
Bug #64979 not fixed yet.
--FILE--
<?php

function new_closure_gen() {
	return function() { 
		static $foo = 0; 
		yield ++$foo; 
	};
}

$closure1 = new_closure_gen();
$closure2 = new_closure_gen();

$gen1 = $closure1();
$gen2 = $closure1();
$gen3 = $closure2();

foreach (array($gen1, $gen2, $gen3) as $gen) {
  foreach ($gen as $val) {
    print "$val\n";
  }
}

?>
--EXPECT--
int(1)
int(2)
int(1)
