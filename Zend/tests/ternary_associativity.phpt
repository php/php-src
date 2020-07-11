--TEST--
Using ternary associativity is deprecated
--FILE--
<?php

1 ? 2 : 3 ? 4 : 5;   // deprecated
(1 ? 2 : 3) ? 4 : 5; // ok
1 ? 2 : (3 ? 4 : 5); // ok

// While the associativity of ?: is also incorrect, it will not cause a
// functional difference, only some unnecessary checks.
1 ?: 2 ?: 3;   // ok
(1 ?: 2) ?: 3; // ok
1 ?: (2 ?: 3); // ok

1 ?: 2 ? 3 : 4;   // deprecated
(1 ?: 2) ? 3 : 4; // ok
1 ?: (2 ? 3 : 4); // ok

1 ? 2 : 3 ?: 4;   // deprecated
(1 ? 2 : 3) ?: 4; // ok
1 ? 2 : (3 ?: 4); // ok

?>
--EXPECTF--
Deprecated: Unparenthesized `a ? b : c ? d : e` is deprecated. Use either `(a ? b : c) ? d : e` or `a ? b : (c ? d : e)` in %s on line 3

Deprecated: Unparenthesized `a ?: b ? c : d` is deprecated. Use either `(a ?: b) ? c : d` or `a ?: (b ? c : d)` in %s on line 13

Deprecated: Unparenthesized `a ? b : c ?: d` is deprecated. Use either `(a ? b : c) ?: d` or `a ? b : (c ?: d)` in %s on line 17
