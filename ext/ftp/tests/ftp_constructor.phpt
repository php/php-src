--TEST--
Attempt to instantiate an FTPConnection directly
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php

try {
    new FTPConnection();
} catch (Error $ex) {
    echo "Exception: ", $ex->getMessage(), "\n";
}
--EXPECT--
Exception: Cannot directly construct FTPConnection, use ftp_connect() or ftp_ssl_connect() instead
