--TEST--
Attempt to instantiate an IMAP\Connection directly
--EXTENSIONS--
imap
--FILE--
<?php

try {
    new IMAP\Connection();
} catch (Error $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
Exception: Cannot directly construct IMAP\Connection, use imap_open() instead
