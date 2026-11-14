--TEST--
Io\Terminal\Terminal: readLine line ending, whitespace, buffering, and EOF stream contracts
--FILE--
<?php

use Io\Terminal\Terminal;

// Class reflection
$rc = new ReflectionClass(Terminal::class);
var_dump($rc->isFinal());
$reflection = new ReflectionMethod(Terminal::class, 'readLine');
var_dump($reflection->getNumberOfParameters());
var_dump((string) $reflection->getReturnType());

// 1. Argument validation: readLine takes 0 parameters
$fp = fopen('php://temp', 'r+');
$terminal = Terminal::fromStreams($fp);
try {
    $terminal->readLine('extra');
    echo "FAIL: readLine accepted argument\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

// 2. Immediate EOF returns null
var_dump($terminal->readLine());

// 3. Ordinary completed line ("hello\n")
fwrite($fp, "hello\n");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 4. CRLF line ending ("world\r\n")
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "world\r\n");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 5. Empty line ("\n" and "\r\n")
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "\n\r\n");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 6. Leading and trailing whitespace preserved
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "   spaced line   \n");
rewind($fp);
var_dump($terminal->readLine());

// 7. Tabs preserved
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "\t\tindented with tabs\t\t\r\n");
rewind($fp);
var_dump($terminal->readLine());

// 8. Unterminated final line at EOF
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "final unterminated");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 9. Multiple sequential lines
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "alpha\nbeta\r\ngamma\ndelta");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 10. Unicode line behavior
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "こんにちは世界\nCafé au lait\r\n🦀 Rust & 🐘 PHP\n");
rewind($fp);
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

// 11. Already-buffered php_stream input is not lost
ftruncate($fp, 0);
rewind($fp);
fwrite($fp, "buffer line 1\nbuffer line 2\n");
rewind($fp);
// Read single character via standard PHP stream function to populate php_stream internal buffer
$c = fgetc($fp);
var_dump($c);
// Terminal::readLine must consume from php_stream buffer, not bypass it
var_dump($terminal->readLine());
var_dump($terminal->readLine());
var_dump($terminal->readLine()); // EOF

fclose($fp);

// 12. Memory stream (php://memory)
$mem = fopen('php://memory', 'r+');
fwrite($mem, "memory line\n");
rewind($mem);
$tMem = Terminal::fromStreams($mem);
var_dump($tMem->readLine());
fclose($mem);

// 13. Multiple consecutive empty CRLF lines and trailing CRLF
$fp2 = fopen('php://temp', 'r+');
fwrite($fp2, "\r\n\r\nline with crlf\r\n\r\n");
rewind($fp2);
$t2 = Terminal::fromStreams($fp2);
var_dump($t2->readLine());
var_dump($t2->readLine());
var_dump($t2->readLine());
var_dump($t2->readLine());
var_dump($t2->readLine()); // EOF

// 14. Extended Unicode, surrogate pairs, and combining sequences
ftruncate($fp2, 0);
rewind($fp2);
fwrite($fp2, "🐘🦀🚀\r\nHello \u{1F468}\u{200D}\u{1F469}\u{200D}\u{1F467}\u{200D}\u{1F466} Family\r\ne\u{0301}cole\n");
rewind($fp2);
var_dump($t2->readLine());
var_dump($t2->readLine());
var_dump($t2->readLine());
var_dump($t2->readLine()); // EOF

// 15. Mode restoration & terminal state usability after raw mode on non-terminal stream
$token2 = $t2->enableRawMode();
$t2->restoreMode($token2);
ftruncate($fp2, 0);
rewind($fp2);
fwrite($fp2, "post restoration line\r\n");
rewind($fp2);
var_dump($t2->readLine());
fclose($fp2);

?>
--EXPECT--
bool(true)
int(0)
string(7) "?string"
ArgumentCountError: Io\Terminal\Terminal::readLine() expects exactly 0 arguments, 1 given
NULL
string(5) "hello"
NULL
string(5) "world"
NULL
string(0) ""
string(0) ""
NULL
string(17) "   spaced line   "
string(22) "		indented with tabs		"
string(18) "final unterminated"
NULL
string(5) "alpha"
string(4) "beta"
string(5) "gamma"
string(5) "delta"
NULL
string(21) "こんにちは世界"
string(13) "Café au lait"
string(20) "🦀 Rust & 🐘 PHP"
NULL
string(1) "b"
string(12) "uffer line 1"
string(13) "buffer line 2"
NULL
string(11) "memory line"
string(0) ""
string(0) ""
string(14) "line with crlf"
string(0) ""
NULL
string(12) "🐘🦀🚀"
string(38) "Hello 👨‍👩‍👧‍👦 Family"
string(7) "école"
NULL
string(21) "post restoration line"
