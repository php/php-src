--TEST--
Verify yield from on generators being properly cycle collected
--FILE--
<?php

function root() {
	global $gens; // create cyclic reference to root
	try {
		yield 1;
	} finally {
		var_dump($gens);
	}
}

function gen($x) {
	global $gens;
	yield from $gens[] = $x ? gen(--$x) : root();
}

$gen = $gens[] = gen(2);
var_dump($gen->current());
unset($gen, $gens);
print "collect\n";
gc_collect_cycles();
print "end\n";

?>
--EXPECT--
int(1)
collect
array(4) {
  [0]=>
  object(Generator)#1 (0) {
  }
  [1]=>
  object(Generator)#2 (0) {
  }
  [2]=>
  object(Generator)#3 (0) {
  }
  [3]=>
  object(Generator)#4 (0) {
  }
}
end
