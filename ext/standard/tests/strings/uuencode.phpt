--TEST--
uuencode family tests
--FILE--
<?php

var_dump(convert_uuencode(array()));
var_dump(convert_uudecode(array()));

var_dump(convert_uuencode(""));
var_dump(convert_uudecode(""));
var_dump($enc = convert_uuencode("~!@#$%^&*()_}{POIUYTREWQQSDFGHJKL:<MNBVCXZ"));
var_dump(convert_uudecode("!@#$%^YUGFDFGHJKLUYTFBNMLOYT"));
var_dump(convert_uudecode($enc));
var_dump($enc = convert_uuencode("not very sophisticated"));
var_dump(convert_uudecode($enc));
var_dump(convert_uudecode(substr($enc, 0, -10)));

echo "Done\n";
?>
--EXPECTF--
Warning: convert_uuencode() expects parameter 1 to be string, array given in %s on line %d
bool(false)

Warning: convert_uudecode() expects parameter 1 to be string, array given in %s on line %d
bool(false)
bool(false)
bool(false)
string(60) "J?B%`(R0E7B8J*"E??7M03TE5651215=145-$1D=(2DM,.CQ-3D)60UA:
`
"
string(1) "%s"
string(42) "~!@#$%^&*()_}{POIUYTREWQQSDFGHJKL:<MNBVCXZ"
string(36) "6;F]T('9E<GD@<V]P:&ES=&EC871E9```
`
"
string(22) "not very sophisticated"

Warning: convert_uudecode(): The given parameter is not a valid uuencoded string in %s on line %d
bool(false)
Done
