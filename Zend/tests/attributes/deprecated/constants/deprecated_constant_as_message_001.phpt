--TEST--
#[\Deprecated]: Using the value of a deprecated constant as the deprecation message.
--FILE--
<?php

#[\Deprecated(TEST)]
const TEST = "from itself";

#[\Deprecated]
const TEST2 = "from another";

#[\Deprecated(TEST2)]
const TEST3 = 1;

TEST;
TEST3;

?>
--EXPECTF--
Deprecated: Constant TEST is deprecated, from itself in %s on line %d

Deprecated: Constant TEST2 is deprecated in %s on line %d

Deprecated: Constant TEST3 is deprecated, from another in %s on line %d
