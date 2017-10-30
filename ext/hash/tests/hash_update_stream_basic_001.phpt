--TEST--
Ensure hash_update_stream() always returns the same hash when $length = 0
--CREDITS--
Symeon Charalabides <symeon@systasis.com> - @phpdublin
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
    die('SKIP - php version too low.');
}
if (!extension_loaded('hash')) die('skip hash extension not available');
if (!extension_loaded('openssl')) die('skip openssl extension not available');
require_once(dirname(__FILE__) . '/skip_mhash.inc'); ?>
?>
--FILE--
<?php

for ($j=0; $j<3; $j++)
{
  // Create pseudo-random hash
  $bytes = openssl_random_pseudo_bytes(15, $cstrong);
  $hash   = sha1(bin2hex($bytes));
  
  // Create temp file with hash
  $fp = tmpfile();
  fwrite($fp, $hash);
  rewind($fp);
  
  // Stream it with 0 length and output hash
  $ctx = hash_init('md5');
  hash_update_stream($ctx, $fp, 0);
  echo hash_final($ctx) . "\n";
}

?>
--EXPECT--
d41d8cd98f00b204e9800998ecf8427e
d41d8cd98f00b204e9800998ecf8427e
d41d8cd98f00b204e9800998ecf8427e

