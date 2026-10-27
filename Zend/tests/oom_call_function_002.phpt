--TEST--
OOM zend_call_function() after OOM: destructor called by an internal function
--INI--
memory_limit=8M
--FILE--
<?php

class A {
    public function __destruct() {
        echo "not reached\n";
    }
}

/* array_splice() allocates the new table first, which crosses the soft limit,
 * then destroys the removed element. The destructor call goes through
 * zend_call_known_function(), which used to bailout with "Couldn't execute method"
 * instead of the OOM. */
$n = 200000;
$arr = range(1, $n);
$arr[$n - 1] = new A;
array_splice($arr, $n - 1, 1);

echo "not reached\n";

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
