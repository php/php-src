--TEST--
__exists: recursion guard is per-property; checking a different property is allowed
--FILE--
<?php

/* The recursion guard prevents re-entering __exists() for the same
 * property while a check for it is in flight. It must NOT short-circuit
 * checks for a different property: __exists('a') triggering a check on
 * $this->b should call __exists('b') for real. */

class C {
    public function __exists(string $n): bool {
        echo "  __exists($n)\n";
        if ($n === 'a') {
            isset($this->b);
        }
        return false;
    }
}

$c = new C;
var_dump(isset($c->a));

?>
--EXPECT--
  __exists(a)
  __exists(b)
bool(false)
