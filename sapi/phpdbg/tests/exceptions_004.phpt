--TEST--
GH-22168 (Exception caught by a later catch is not misreported as uncaught when an earlier catch references a non-existent class)
--PHPDBG--
r
q
--EXPECTF--
[Successful compilation of %s]
prompt> Caught it. 'handled' it
[Script ended normally]
prompt>
--FILE--
<?php declare(strict_types=1);

try {
    throw new RuntimeException();
} catch (LibrarySpecificException) {
    // A library specific exception. Not autoloaded since it did not happen.
} catch (RuntimeException) {
    echo "Caught it. 'handled' it\n";
}
