--TEST--
Test easter_date() on 32bit systems checks the upper year limit
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) die("skip 32-bit only"); ?>
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
    easter_date(2040);
} catch (ValueError $ex) {
    echo "{$ex->getMessage()}\n";
}
?>
--EXPECT--
easter_date(): Argument #1 ($year) must be between 1970 and 2037 (inclusive)
