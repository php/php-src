--TEST--
Issue #140: "opcache.enable_file_override" doesn't respect "opcache.revalidate_freq"
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.revalidate_freq=0
opcache.file_update_protection=0
--EXTENSIONS--
opcache
--SKIPIF--
<?php if (getenv("SKIP_SLOW_TESTS")) die("skip slow tests excluded by request") ?>
--FILE--
<?php
define("FILENAME", __DIR__ . "/issuer0140.inc.php");
file_put_contents(FILENAME, "1\n");

var_dump(is_readable(FILENAME));
include(FILENAME);
var_dump(filemtime(FILENAME));

sleep(2);
file_put_contents(FILENAME, "2\n");

var_dump(is_readable(FILENAME));
include(FILENAME);
var_dump(filemtime(FILENAME));

sleep(2);
unlink(FILENAME);

var_dump(is_readable(FILENAME));
var_dump(@include(FILENAME));
var_dump(@filemtime(FILENAME));
?>
--EXPECTF--
bool(true)
1
int(%d)
bool(true)
2
int(%d)
bool(false)
bool(false)
bool(false)
