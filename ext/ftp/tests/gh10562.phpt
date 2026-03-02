--TEST--
GH-10562 (Memory leak with consecutive ftp_nb_fget)
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
if (!$ftp) die("Couldn't connect to the server");

var_dump(ftp_login($ftp, 'anonymous', 'IEUser@'));

$local_file = __DIR__ . DIRECTORY_SEPARATOR . "gh10562.txt";
$fout = fopen($local_file, "w");

// This requests more data, but we don't do the loop to fetch it.
$ret = ftp_nb_fget($ftp, $fout, "fget", FTP_BINARY, 0);
var_dump($ret == FTP_MOREDATA);

// This aborts the previous request, fetches the whole "a story" file.
$ret = ftp_nb_fget($ftp, $fout, "a story", FTP_BINARY, 0);
while ($ret == FTP_MOREDATA) {
    $ret = ftp_nb_continue($ftp);
}

fclose($fout);

echo file_get_contents($local_file), "\n";
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "gh10562.txt");
?>
--EXPECT--
bool(true)
bool(true)
BINARYFooBar
For sale: baby shoes, never worn.
