--TEST--
Attempt to instantiate an FTP\Connection directly
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php

try {
    new FTP\Connection();
} catch (Error $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
Exception: Cannot directly construct FTP\Connection, use ftp_connect() or ftp_ssl_connect() instead
