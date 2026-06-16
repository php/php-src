--TEST--
There must be a difference between label: try { ... } and try { label: ... }
--FILE--
<?php

label: try {
    goto label;
} finally {
    print "success";
    return; // don't loop
}

?>
--EXPECTF--
Deprecated: Returning from a finally block is deprecated in %s on line %d
success