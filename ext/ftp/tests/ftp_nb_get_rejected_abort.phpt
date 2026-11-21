--TEST--
ftp_nb_get() must clear ftp->nb when aborting a pending transfer fails
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");
var_dump(ftp_login($ftp, 'user', 'pass'));

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_get_rejected_abort.txt";

/* Leaves a transfer pending. The server answers this one with 451, not 226. */
var_dump(ftp_nb_get($ftp, $local_file, "abort_rejected", FTP_BINARY) === FTP_MOREDATA);

/* Aborts the pending transfer; the server's 451 sends ftp_nb_get() to bail. */
var_dump(@ftp_nb_get($ftp, $local_file, "a story", FTP_BINARY) === FTP_FAILED);

/* There is no transfer to continue any more. Before the fix ftp->nb was still
 * set here, so this reached php_stream_close() on an already cleared
 * ftp->stream and crashed. */
var_dump(@ftp_nb_continue($ftp) === FTP_FAILED);

ftp_close($ftp);
echo "closed\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "ftp_nb_get_rejected_abort.txt");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
closed
