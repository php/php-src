--TEST--
Io\Terminal\Terminal: CRLF consumption across secret boundaries and chunk splits
--SKIPIF--
<?php
if (!class_exists(\Io\Terminal\Terminal::class)) die('skip Io\Terminal\Terminal not available');
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Time\Duration;

// Part 1: All in one buffer
$fp = fopen('php://memory', 'r+');
fwrite($fp, "secretA\r\nsecretB\r\nK");
rewind($fp);

$term1 = Terminal::fromStreams($fp);
$s1 = $term1->readSecret();
$s2 = $term1->readSecret();
$k1 = $term1->readKey();

var_dump($s1 === "secretA");
var_dump($s2 === "secretB");
var_dump($k1 === "K");
fclose($fp);

// Part 2: Split CRLF across write chunks
$pair = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
$readStream = $pair[0];
$writeStream = $pair[1];
$term2 = Terminal::fromStreams($readStream);

// First write terminates with \r only
fwrite($writeStream, "alpha\r");
$sAlpha = $term2->readSecret();
var_dump($sAlpha === "alpha");

// Second write begins with the split \n, then beta\r\n, then trailing key
fwrite($writeStream, "\nbeta\r\nQ");
$sBeta = $term2->readSecret();
var_dump($sBeta === "beta");

$kQ = $term2->readKey(Duration::fromMilliseconds(100));
var_dump($kQ === "Q");

fclose($readStream);
fclose($writeStream);

// Part 3: Split CRLF followed by readLine()
$pair3 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
$readStream3 = $pair3[0];
$writeStream3 = $pair3[1];
$term3 = Terminal::fromStreams($readStream3);

fwrite($writeStream3, "password\r");
$sPass = $term3->readSecret();
var_dump($sPass === "password");

// Next chunk arrives with split \n, then followed by line content and newline
fwrite($writeStream3, "\nmy input line\n");
$line3 = $term3->readLine();
var_dump($line3 === "my input line");

// Part 4: Chunk size 1, CR then later "next\n"
$p4 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p4[0], 1);
$t4 = Terminal::fromStreams($p4[0]);
fwrite($p4[1], "password\r");
var_dump($t4->readSecret() === "password");
fwrite($p4[1], "next\n");
var_dump($t4->readLine() === "next");
fclose($p4[0]);
fclose($p4[1]);

// Part 5: Chunk size 1, CR then later "\nnext\n"
$p5 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p5[0], 1);
$t5 = Terminal::fromStreams($p5[0]);
fwrite($p5[1], "password\r");
var_dump($t5->readSecret() === "password");
fwrite($p5[1], "\nnext\n");
var_dump($t5->readLine() === "next");
fclose($p5[0]);
fclose($p5[1]);

// Part 6: Chunk size 1, CR with next line already queued in same write
$p6 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p6[0], 1);
$t6 = Terminal::fromStreams($p6[0]);
fwrite($p6[1], "password\rnext\n");
var_dump($t6->readSecret() === "password");
var_dump($t6->readLine() === "next");
fclose($p6[0]);
fclose($p6[1]);

// Part 7: Chunk size 1, read "password\r", then supply "\r\nnext\n" -> readLine() returns "" and "next"
$p7 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p7[0], 1);
$t7 = Terminal::fromStreams($p7[0]);
fwrite($p7[1], "password\r");
var_dump($t7->readSecret() === "password");
fwrite($p7[1], "\r\nnext\n");
var_dump($t7->readLine() === "");
var_dump($t7->readLine() === "next");
fclose($p7[0]);
fclose($p7[1]);

// Part 8: A data CR before CRLF must survive split-prefix normalization.
$p8 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p8[0], 1);
$t8 = Terminal::fromStreams($p8[0]);
fwrite($p8[1], "password\r");
var_dump($t8->readSecret() === "password");
fwrite($p8[1], "\r\r\nnext\n");
var_dump($t8->readLine() === "\r");
var_dump($t8->readLine() === "next");
fclose($p8[0]);
fclose($p8[1]);

// Part 9: A bare saved CR at EOF remains unterminated line data.
$p9 = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
stream_set_chunk_size($p9[0], 1);
$t9 = Terminal::fromStreams($p9[0]);
fwrite($p9[1], "password\r");
var_dump($t9->readSecret() === "password");
fwrite($p9[1], "\r");
fclose($p9[1]);
var_dump($t9->readLine() === "\r");
var_dump($t9->readLine() === null);
fclose($p9[0]);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
