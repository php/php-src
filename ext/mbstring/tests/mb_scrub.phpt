--TEST--
mb_scrub()
--EXTENSIONS--
mbstring
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
