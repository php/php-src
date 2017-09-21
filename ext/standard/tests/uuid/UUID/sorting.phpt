--TEST--
UUID sorting
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$u1 = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");
$u2 = UUID::fromBinary("\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2\2");
$u3 = UUID::fromBinary("\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3\3");

$data = [$u3, $u2, $u1];
sort($data);
var_dump($data === [$u1, $u2, $u3]);

?>
--EXPECT--
bool(true)
