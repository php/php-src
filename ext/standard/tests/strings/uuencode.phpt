--TEST--
uuencode family tests
--FILE--
<?php

var_dump($enc = convert_uuencode(""));
var_dump(convert_uudecode($enc));
var_dump($enc = convert_uuencode("~!@#$%^&*()_}{POIUYTREWQQSDFGHJKL:<MNBVCXZ"));
var_dump(convert_uudecode("!@#$%^YUGFDFGHJKLUYTFBNMLOYT"));
var_dump(convert_uudecode($enc));
var_dump($enc = convert_uuencode("not very sophisticated"));
var_dump(convert_uudecode($enc));
var_dump(convert_uudecode(""));
var_dump(convert_uudecode(substr($enc, 0, -10)));

echo "Done\n";
?>
--EXPECTF--
string(2) "`
"
string(0) ""
string(60) "J?B%`(R0E7B8J*"E??7M03TE5651215=145-$1D=(2DM,.CQ-3D)60UA:
`
"
string(1) "%s"
string(42) "~!@#$%^&*()_}{POIUYTREWQQSDFGHJKL:<MNBVCXZ"
string(36) "6;F]T('9E<GD@<V]P:&ES=&EC871E9```
`
"
string(22) "not very sophisticated"

Warning: convert_uudecode(): Argument #1 ($data) is not a valid uuencoded string in %s on line %d
bool(false)

Warning: convert_uudecode(): Argument #1 ($data) is not a valid uuencoded string in %s on line %d
bool(false)
Done
