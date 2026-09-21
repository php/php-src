--TEST--
Re-entrant ftp_nb_get() from a stream wrapper during an active non-blocking ftp_nb_get() must not corrupt the outer transfer
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

class NbGetDuringNbGet {
    public $context;
    public static $ftp;
    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }
    public function stream_write($data) {
        @ftp_nb_get(self::$ftp, 'php://memory', 'a story.txt', FTP_BINARY);
        return strlen($data);
    }
    public function stream_close() {}
    public function stream_eof() {
        return true;
    }
}

stream_wrapper_register('reentrantnbget', NbGetDuringNbGet::class);

$ftp = ftp_connect('127.0.0.1', $port);
var_dump(ftp_login($ftp, 'user', 'pass'));
NbGetDuringNbGet::$ftp = $ftp;

$r = @ftp_nb_get($ftp, 'reentrantnbget://sink', 'a story.txt', FTP_BINARY);
while ($r == FTP_MOREDATA) {
    $r = @ftp_nb_continue($ftp);
}
var_dump($r === FTP_FINISHED);

ftp_close($ftp);
echo "closed\n";
?>
--EXPECT--
bool(true)
bool(true)
closed
