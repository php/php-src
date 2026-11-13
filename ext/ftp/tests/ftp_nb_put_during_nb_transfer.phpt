--TEST--
ftp_nb_put() must abort a pending non-blocking transfer, like ftp_nb_get() does
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");
var_dump(ftp_login($ftp, 'user', 'pass'));

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_put_during_nb_transfer.txt";
$fout = fopen($local_file, "w");

/* Requests more data, but we deliberately do not loop to fetch it, so a
 * transfer is still pending when the upload below starts. */
var_dump(ftp_nb_fget($ftp, $fout, "fget", FTP_BINARY, 0) === FTP_MOREDATA);
fclose($fout);

/* Without the guard in ftp_nb_put() this reuses ftp->data while the previous
 * databuf_t is still owned by it, and reads the pending transfer's response
 * as the answer to its own command. */
$ret = ftp_nb_put($ftp, basename(__FILE__), __FILE__, FTP_ASCII);
while ($ret == FTP_MOREDATA) {
    $ret = ftp_nb_continue($ftp);
}
var_dump($ret === FTP_FINISHED);

ftp_close($ftp);
echo "closed\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_put_during_nb_transfer.txt");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
closed
