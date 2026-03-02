--TEST--
Verify yield from on generators being properly cycle collected
--INI--
zend.enable_gc = 1
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
--EXPECTF--
int(1)
collect
array(4) {
  [0]=>
  object(Generator)#%d (1) {
    ["function"]=>
    string(3) "gen"
  }
  [1]=>
  object(Generator)#%d (1) {
    ["function"]=>
    string(3) "gen"
  }
  [2]=>
  object(Generator)#%d (1) {
    ["function"]=>
    string(3) "gen"
  }
  [3]=>
  object(Generator)#%d (1) {
    ["function"]=>
    string(4) "root"
  }
}
end
