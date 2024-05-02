--TEST--
Testing ftp_fput basic functionality
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
$ftp or die("Couldn't connect to the server");

$filename = "large_file.txt";
$filepath = __DIR__ . "/" . $filename;

// Test on boundary of 2 buffers
for ($i = 4094; $i < 4098; $i++) {
    $contents = str_repeat("a", $i) . "\n" . str_repeat("b", 10);
    file_put_contents($filepath, $contents);
    var_dump(ftp_put($ftp, "large_file.txt", $filepath, FTP_ASCII));
}

?>
--CLEAN--
<?php
@unlink(__DIR__ . "/large_file.txt");
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
