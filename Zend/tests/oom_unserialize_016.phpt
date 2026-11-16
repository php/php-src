--TEST--
OOM unserialize() cleans up BG(unserialize) after a delayed OOM
--INI--
memory_limit=8M
--FILE--
<?php

register_shutdown_function(function () {
    /* Runs after the fatal error. If the var_hash of the failed call were
     * still installed, this call would nest into it and R:1 would resolve
     * into that call's table instead of this array. */
    var_dump(unserialize('a:1:{i:0;R:1;}'));
});

/* 40000 one-element arrays: ~760 KB serialized, ~10 MB unserialized. */
$n = 40000;
$ser = 'a:' . $n . ':{';
for ($i = 0; $i < $n; $i++) {
    $ser .= 'i:' . $i . ';a:1:{i:0;i:0;}';
}
$ser .= '}';

unserialize($ser);

echo "not reached\n";

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
array(1) {
  [0]=>
  *RECURSION*
}
