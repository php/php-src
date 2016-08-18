--TEST--
mb_scrub()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--FILE--
<?php
var_dump(
    "?" === mb_scrub("\x80"),
    "?" === mb_scrub("\x80", 'UTF-8')
);
?>
--EXPECT--
bool(true)
bool(true)
