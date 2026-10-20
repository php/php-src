--TEST--
Ensures an exception is thrown if versions are compared with an invalid operator
--CREDITS--
David Stockton - <dave@davidstockton.com> - i3logix PHP Testfest 2017
--FILE--
<?php
try {
    version_compare('1.2', '2.1', '??');
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: version_compare(): Argument #3 ($operator) must be a valid comparison operator
