--TEST--
Io\Terminal\Terminal: Windows console and stream readLine contracts
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') {
    die("skip Windows-only test");
}
?>
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\ModeToken;

// 1. CRLF normalization and empty line contracts on Windows
$fp = fopen('php://temp', 'r+');
$terminal = Terminal::fromStreams($fp);

fwrite($fp, "windows line 1\r\n\r\nwindows line 2\r\n");
rewind($fp);

var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 2. Windows Unicode line handling with surrogate pairs
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "🐘 PHP \u{1F980} Rust\r\n東京\r\n");
rewind($fp);

var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 3. No-op raw mode lease on non-terminal stream
$token = $terminal->enableRawMode();
var_dump($token instanceof ModeToken);
$terminal->restoreMode($token);
echo "restored non-tty token\n";

ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "usable after mode check\r\n");
rewind($fp);
var_dump($terminal->readLine());

fclose($fp);

?>
--EXPECT--
string(14) "windows line 1"
string(0) ""
string(14) "windows line 2"
NULL
string(18) "🐘 PHP 🦀 Rust"
string(6) "東京"
NULL
bool(true)
restored non-tty token
string(23) "usable after mode check"
