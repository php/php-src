--TEST--
Test easter_date() on 64bit systems checks the upper year limit
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip 64-bit only"); ?>
--INI--
date.timezone=UTC
--ENV--
TZ=UTC
--EXTENSIONS--
calendar
--FILE--
<?php
putenv('TZ=UTC');
try {
    easter_date(293274701009);
} catch (ValueError $ex) {
    echo $ex::class, ': ', $ex->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: easter_date(): Argument #1 ($year) must be a year before 2.000.000.000 (inclusive)
