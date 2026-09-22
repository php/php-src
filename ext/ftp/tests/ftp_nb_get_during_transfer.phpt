--TEST--
Re-entrant ftp_nb_get() from a stream wrapper during a blocking ftp_get() must not free the active data connection
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

class NbGetDuringGet {
    public $context;
    public static $ftp;
    public static $error;
    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }
    public function stream_write($data) {
        try {
            ftp_nb_get(self::$ftp, 'php://memory', 'a story.txt', FTP_BINARY);
        } catch (Throwable $e) {
            /* recorded rather than echoed: stream_write() may run more than once */
            self::$error = $e::class . ': ' . $e->getMessage();
        }
        return strlen($data);
    }
    public function stream_close() {}
    public function stream_eof() {
        return true;
    }
}

stream_wrapper_register('reentrantget', NbGetDuringGet::class);

$ftp = ftp_connect('127.0.0.1', $port);
var_dump(ftp_login($ftp, 'user', 'pass'));
NbGetDuringGet::$ftp = $ftp;

var_dump(@ftp_get($ftp, 'reentrantget://sink', 'a story.txt', FTP_BINARY));

var_dump(NbGetDuringGet::$error);

ftp_close($ftp);
echo "closed\n";
?>
--EXPECT--
bool(true)
bool(true)
string(68) "Error: Cannot start a transfer while another transfer is in progress"
closed
