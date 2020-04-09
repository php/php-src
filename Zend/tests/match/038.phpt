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
Fatal error: Match expressions may only contain one default arm in %s on line 7
