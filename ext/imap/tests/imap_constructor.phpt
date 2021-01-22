--TEST--
Attempt to instantiate an IMAPConnection directly
--SKIPIF--
<?php
extension_loaded('imap') or die('skip imap extension not available in this build');
--FILE--
<?php

try {
    new IMAPConnection();
} catch (Error $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
Exception: Cannot directly construct IMAPConnection, use imap_open() instead
