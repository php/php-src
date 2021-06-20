--TEST--
Attempt to instantiate an IMAP\Connection directly
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
--FILE--
<?php

try {
    new IMAP\Connection();
} catch (Error $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
Exception: Cannot directly construct IMAP\Connection, use imap_open() instead
