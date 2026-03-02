--TEST--
Allowed nested ternary cases
--FILE--
<?php

(1 ? 2 : 3) ? 4 : 5; // ok
1 ? 2 : (3 ? 4 : 5); // ok

1 ?: 2 ?: 3;   // ok
(1 ?: 2) ?: 3; // ok
1 ?: (2 ?: 3); // ok

(1 ?: 2) ? 3 : 4; // ok
1 ?: (2 ? 3 : 4); // ok

(1 ? 2 : 3) ?: 4; // ok
1 ? 2 : (3 ?: 4); // ok

?>
===DONE===
--EXPECT--
===DONE===
