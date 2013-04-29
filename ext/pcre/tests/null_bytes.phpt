--TEST--
Zero byte test
--FILE--
<?php

preg_match("\0//i", "");
preg_match("/\0/i", "");
preg_match("//\0i", "");
preg_match("//i\0", "");
preg_match("/\\\0/i", "");

preg_match("\0[]i", "");
preg_match("[\0]i", "");
preg_match("[]\0i", "");
preg_match("[]i\0", "");
preg_match("[\\\0]i", "");

preg_replace("/foo/e\0/i", "echo('Eek');", "");

?>
--EXPECTF--
Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 3

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 4

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 5

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 6

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 7

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 9

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 10

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 11

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 12

Warning: preg_match(): Null byte in regex in %snull_bytes.php on line 13

Warning: preg_replace(): Null byte in regex in %snull_bytes.php on line 15
