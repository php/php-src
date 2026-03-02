--TEST--
Testing whether ftp_nb_continue() fetches more data
--EXTENSIONS--
ftp
pcntl
--FILE--
<?php
require 'server.inc';

$file = "mediumfile.txt";

$ftp = ftp_connect('127.0.0.1', $port);
ftp_login($ftp, 'user', 'pass');
if (!$ftp) die("Couldn't connect to the server");

$local_file = __DIR__ . DIRECTORY_SEPARATOR . $file;
touch($local_file);

$r = ftp_nb_get($ftp, $local_file, $file, FTP_BINARY);
while ($r == FTP_MOREDATA) {
    $r = ftp_nb_continue($ftp);
}
ftp_close($ftp);

echo file_get_contents($local_file);
?>
--CLEAN--
<?php
@unlink(__DIR__ . DIRECTORY_SEPARATOR . "mediumfile.txt");
?>
--EXPECT--
This is line 0 of the test data.
This is line 1 of the test data.
This is line 2 of the test data.
This is line 3 of the test data.
This is line 4 of the test data.
This is line 5 of the test data.
This is line 6 of the test data.
This is line 7 of the test data.
This is line 8 of the test data.
This is line 9 of the test data.
This is line 10 of the test data.
This is line 11 of the test data.
This is line 12 of the test data.
This is line 13 of the test data.
This is line 14 of the test data.
This is line 15 of the test data.
This is line 16 of the test data.
This is line 17 of the test data.
This is line 18 of the test data.
This is line 19 of the test data.
This is line 20 of the test data.
This is line 21 of the test data.
This is line 22 of the test data.
This is line 23 of the test data.
This is line 24 of the test data.
This is line 25 of the test data.
This is line 26 of the test data.
This is line 27 of the test data.
This is line 28 of the test data.
This is line 29 of the test data.
This is line 30 of the test data.
This is line 31 of the test data.
This is line 32 of the test data.
This is line 33 of the test data.
This is line 34 of the test data.
This is line 35 of the test data.
This is line 36 of the test data.
This is line 37 of the test data.
This is line 38 of the test data.
This is line 39 of the test data.
This is line 40 of the test data.
This is line 41 of the test data.
This is line 42 of the test data.
This is line 43 of the test data.
This is line 44 of the test data.
This is line 45 of the test data.
This is line 46 of the test data.
This is line 47 of the test data.
This is line 48 of the test data.
This is line 49 of the test data.
This is line 50 of the test data.
This is line 51 of the test data.
This is line 52 of the test data.
This is line 53 of the test data.
This is line 54 of the test data.
This is line 55 of the test data.
This is line 56 of the test data.
This is line 57 of the test data.
This is line 58 of the test data.
This is line 59 of the test data.
This is line 60 of the test data.
This is line 61 of the test data.
This is line 62 of the test data.
This is line 63 of the test data.
This is line 64 of the test data.
This is line 65 of the test data.
This is line 66 of the test data.
This is line 67 of the test data.
This is line 68 of the test data.
This is line 69 of the test data.
This is line 70 of the test data.
This is line 71 of the test data.
This is line 72 of the test data.
This is line 73 of the test data.
This is line 74 of the test data.
This is line 75 of the test data.
This is line 76 of the test data.
This is line 77 of the test data.
This is line 78 of the test data.
This is line 79 of the test data.
This is line 80 of the test data.
This is line 81 of the test data.
This is line 82 of the test data.
This is line 83 of the test data.
This is line 84 of the test data.
This is line 85 of the test data.
This is line 86 of the test data.
This is line 87 of the test data.
This is line 88 of the test data.
This is line 89 of the test data.
This is line 90 of the test data.
This is line 91 of the test data.
This is line 92 of the test data.
This is line 93 of the test data.
This is line 94 of the test data.
This is line 95 of the test data.
This is line 96 of the test data.
This is line 97 of the test data.
This is line 98 of the test data.
This is line 99 of the test data.
This is line 100 of the test data.
This is line 101 of the test data.
This is line 102 of the test data.
This is line 103 of the test data.
This is line 104 of the test data.
This is line 105 of the test data.
This is line 106 of the test data.
This is line 107 of the test data.
This is line 108 of the test data.
This is line 109 of the test data.
This is line 110 of the test data.
This is line 111 of the test data.
This is line 112 of the test data.
This is line 113 of the test data.
This is line 114 of the test data.
This is line 115 of the test data.
This is line 116 of the test data.
This is line 117 of the test data.
This is line 118 of the test data.
This is line 119 of the test data.
This is line 120 of the test data.
This is line 121 of the test data.
This is line 122 of the test data.
This is line 123 of the test data.
This is line 124 of the test data.
This is line 125 of the test data.
This is line 126 of the test data.
This is line 127 of the test data.
This is line 128 of the test data.
This is line 129 of the test data.
This is line 130 of the test data.
This is line 131 of the test data.
This is line 132 of the test data.
This is line 133 of the test data.
This is line 134 of the test data.
This is line 135 of the test data.
This is line 136 of the test data.
This is line 137 of the test data.
This is line 138 of the test data.
This is line 139 of the test data.
This is line 140 of the test data.
This is line 141 of the test data.
This is line 142 of the test data.
This is line 143 of the test data.
This is line 144 of the test data.
This is line 145 of the test data.
This is line 146 of the test data.
This is line 147 of the test data.
This is line 148 of the test data.
This is line 149 of the test data.
