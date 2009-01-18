--TEST--
sha1() with ASCII output.
--FILE--
<?php

/* Prototype: string sha1  ( string $str  [, bool $raw_output  ] )
 * Description: Calculate the sha1 hash of a string
 */

echo "*** Testing sha1() : basic functionality ***\n";

echo "\n-- Without raw argument --\n";
var_dump(sha1(b""));
var_dump(sha1(b"a"));
var_dump(sha1(b"abc"));
var_dump(sha1(b"message digest"));
var_dump(sha1(b"abcdefghijklmnopqrstuvwxyz"));
var_dump(sha1(b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
var_dump(sha1(b"12345678901234567890123456789012345678901234567890123456789012345678901234567890"));

echo "\n-- With raw == false --\n";
var_dump(sha1(b"", false));
var_dump(sha1(b"a", false));
var_dump(sha1(b"abc", false));
var_dump(sha1(b"message digest", false));
var_dump(sha1(b"abcdefghijklmnopqrstuvwxyz", false));
var_dump(sha1(b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", false));
var_dump(sha1(b"12345678901234567890123456789012345678901234567890123456789012345678901234567890", false));

echo "\n-- With raw == true --\n";
var_dump(bin2hex(sha1(b"", true)));
var_dump(bin2hex(sha1(b"a", true)));
var_dump(bin2hex(sha1(b"abc", true)));
var_dump(bin2hex(sha1(b"message digest", true)));
var_dump(bin2hex(sha1(b"abcdefghijklmnopqrstuvwxyz", true)));
var_dump(bin2hex(sha1(b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789", true)));
var_dump(bin2hex(sha1(b"12345678901234567890123456789012345678901234567890123456789012345678901234567890", true)));

?>
===DONE===
--EXPECT--
*** Testing sha1() : basic functionality ***

-- Without raw argument --
unicode(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
unicode(40) "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"
unicode(40) "a9993e364706816aba3e25717850c26c9cd0d89d"
unicode(40) "c12252ceda8be8994d5fa0290a47231c1d16aae3"
unicode(40) "32d10c7b8cf96570ca04ce37f2a19d84240d3a89"
unicode(40) "761c457bf73b14d27e9e9265c46f4b4dda11f940"
unicode(40) "50abf5706a150990a08b2c5ea40fa0e585554732"

-- With raw == false --
unicode(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
unicode(40) "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"
unicode(40) "a9993e364706816aba3e25717850c26c9cd0d89d"
unicode(40) "c12252ceda8be8994d5fa0290a47231c1d16aae3"
unicode(40) "32d10c7b8cf96570ca04ce37f2a19d84240d3a89"
unicode(40) "761c457bf73b14d27e9e9265c46f4b4dda11f940"
unicode(40) "50abf5706a150990a08b2c5ea40fa0e585554732"

-- With raw == true --
unicode(40) "da39a3ee5e6b4b0d3255bfef95601890afd80709"
unicode(40) "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"
unicode(40) "a9993e364706816aba3e25717850c26c9cd0d89d"
unicode(40) "c12252ceda8be8994d5fa0290a47231c1d16aae3"
unicode(40) "32d10c7b8cf96570ca04ce37f2a19d84240d3a89"
unicode(40) "761c457bf73b14d27e9e9265c46f4b4dda11f940"
unicode(40) "50abf5706a150990a08b2c5ea40fa0e585554732"
===DONE===