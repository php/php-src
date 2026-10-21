--TEST--
OOM zend_call_function() after OOM: delayed __unserialize() call
--INI--
memory_limit=8M
--FILE--
<?php

class U {
    public function __unserialize(array $data): void {
        echo "not reached\n";
    }
}

register_shutdown_function(function () {
    /* The delayed call happens in var_destroy(), which used to bail out with
     * "Couldn't execute method" */
    var_dump(unserialize('a:1:{i:0;R:1;}'));
});

/* An object with __unserialize(), then 40000 one-element arrays: ~760 KB
 * serialized, ~10 MB unserialized. */
$n = 40000;
$ser = 'a:' . ($n + 1) . ':{i:0;O:1:"U":1:{s:1:"a";i:1;}';
for ($i = 1; $i <= $n; $i++) {
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
