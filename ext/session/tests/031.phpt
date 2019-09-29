--TEST--
setting hash_function to sha512 and hash_bits_per_character > 4 should not crash
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
session.hash_function=sha512
session.hash_bits_per_character=5
--FILE--
<?php
error_reporting(E_ALL);

session_start();
session_regenerate_id(TRUE);

print "I live\n";
?>
--EXPECT--
I live
