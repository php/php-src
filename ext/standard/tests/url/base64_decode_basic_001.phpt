--TEST--
Test base64_decode() function : basic functionality - ensure all base64 alphabet is supported.
--FILE--
<?php
/* Prototype  : proto string base64_decode(string str[, bool strict])
 * Description: Decodes string using MIME base64 algorithm
 * Source code: ext/standard/base64.c
 * Alias to functions:
 */

echo "Decode an input string containing the whole base64 alphabet:\n";
$allbase64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/VV==";
var_dump(bin2hex(base64_decode($allbase64)));
var_dump(bin2hex(base64_decode($allbase64, false)));
var_dump(bin2hex(base64_decode($allbase64, true)));

echo "Done";
?>
--EXPECTF--
Decode an input string containing the whole base64 alphabet:
string(98) "00108310518720928b30d38f41149351559761969b71d79f8218a39259a7a29aabb2dbafc31cb3d35db7e39ebbf3dfbf55"
string(98) "00108310518720928b30d38f41149351559761969b71d79f8218a39259a7a29aabb2dbafc31cb3d35db7e39ebbf3dfbf55"
string(98) "00108310518720928b30d38f41149351559761969b71d79f8218a39259a7a29aabb2dbafc31cb3d35db7e39ebbf3dfbf55"
Done
