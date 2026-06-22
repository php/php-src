--TEST--
ftp_close() from a stream wrapper during a non-blocking transfer throws instead of freeing the connection
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

class CloseDuringNbWrite {
    public $context;
    public static $ftp;
    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }
    public function stream_write($data) {
        ftp_close(self::$ftp);
        return strlen($data);
    }
    public function stream_close() {}
    public function stream_eof() {
        return true;
    }
}

stream_wrapper_register('reentrantnb', CloseDuringNbWrite::class);

$ftp = ftp_connect('127.0.0.1', $port);
var_dump(ftp_login($ftp, 'user', 'pass'));
CloseDuringNbWrite::$ftp = $ftp;

try {
    @ftp_nb_get($ftp, 'reentrantnb://sink', 'a story.txt', FTP_BINARY);
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

ftp_close($ftp);
echo "closed\n";
?>
--EXPECT--
bool(true)
Cannot close FTP\Connection while a transfer is in progress
closed
