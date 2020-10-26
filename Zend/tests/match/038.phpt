--TEST--
Test multiple default arms in match in different arms
--FILE--
<?php

match (1) {
    default => 'foo',
    1 => 'bar',
    2 => 'baz',
    default => 'qux',
};

?>
--EXPECTF--
Fatal error: Match expressions cannot contain more than one default arms in %s on line %d
