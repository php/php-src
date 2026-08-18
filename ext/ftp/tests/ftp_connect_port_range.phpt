--TEST--
ftp_connect(): $port values outside 0-65535 must throw ValueError, not alias onto a valid port
--EXTENSIONS--
ftp
--FILE--
<?php
foreach ([-1, 65536, 65536 + 2121, PHP_INT_MIN, PHP_INT_MAX] as $port) {
    try {
        ftp_connect('127.0.0.1', $port);
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

// Port 0 must be accepted (maps to default FTP port 21 in ftp_open)
// We expect false (connection refused), not ValueError.
$result = @ftp_connect('127.0.0.1', 0);
var_dump($result === false || is_object($result)); // true either way

// Port 65535 must be accepted (last valid port).
$result = @ftp_connect('127.0.0.1', 65535);
var_dump($result === false || is_object($result)); // true either way
?>
--EXPECT--
ftp_connect(): Argument #2 ($port) must be between 0 and 65535
ftp_connect(): Argument #2 ($port) must be between 0 and 65535
ftp_connect(): Argument #2 ($port) must be between 0 and 65535
ftp_connect(): Argument #2 ($port) must be between 0 and 65535
ftp_connect(): Argument #2 ($port) must be between 0 and 65535
bool(true)
bool(true)
