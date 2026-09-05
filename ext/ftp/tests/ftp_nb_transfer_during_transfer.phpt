--TEST--
ftp_nb_fget() and ftp_nb_fput() throw when a transfer is already in progress
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

class TransferDuringNbWrite {
    public $context;
    public static $ftp;
    public static $call;
    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }
    public function stream_write($data) {
        try {
            (self::$call)(self::$ftp);
        } catch (\Error $e) {
            echo $e::class, ': ', $e->getMessage(), "\n";
        }
        return strlen($data);
    }
    public function stream_close() {}
    public function stream_eof() {
        return true;
    }
}

stream_wrapper_register('reentrantnb', TransferDuringNbWrite::class);

$ftp = ftp_connect('127.0.0.1', $port);
var_dump(ftp_login($ftp, 'user', 'pass'));
TransferDuringNbWrite::$ftp = $ftp;

$sink = fopen('php://memory', 'w+');

TransferDuringNbWrite::$call = static function ($ftp) use ($sink) {
    ftp_nb_fget($ftp, $sink, 'a story.txt', FTP_BINARY);
};
@ftp_nb_get($ftp, 'reentrantnb://sink', 'a story.txt', FTP_BINARY);

TransferDuringNbWrite::$call = static function ($ftp) use ($sink) {
    ftp_nb_fput($ftp, 'a story.txt', $sink, FTP_BINARY);
};
@ftp_nb_get($ftp, 'reentrantnb://sink', 'a story.txt', FTP_BINARY);

ftp_close($ftp);
echo "closed\n";
?>
--EXPECT--
bool(true)
Error: Cannot start a transfer while another transfer is in progress
Error: Cannot start a transfer while another transfer is in progress
closed
