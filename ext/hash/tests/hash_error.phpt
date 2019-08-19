--TEST--
Hash: hash() function : error conditions
--FILE--
<?php

require_once __DIR__ . '/exceptional.php';

/* Prototype  : string hash  ( string $algo  , string $data  [, bool $raw_output  ] )
 * Description: Generate a hash value (message digest)
 * Source code: ext/hash/hash.c
 * Alias to functions:
*/
echo "*** Testing hash() : error conditions ***\n";

echo "\n-- Testing hash() function with invalid hash algorithm --\n";
trycatch_dump(fn() => hash('foo', ''));

?>
===Done===
--EXPECTF--
Warning: require_once(/mnt/c/users/mark/source/repos/local-php-src/src/ext/hash/tests/exceptional.php): failed to open stream: No such file or directory in %s on line %d

Fatal error: require_once(): Failed opening required '/mnt/c/users/mark/source/repos/local-php-src/src/ext/hash/tests/exceptional.php' (include_path='.:') in %s on line %d
