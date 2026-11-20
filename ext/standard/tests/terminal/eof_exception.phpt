--TEST--
Io\Terminal\Terminal: EofException inheritance, EOF vs open writer, truncated UTF-8, and readLine contracts
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\TerminalException;
use Io\Terminal\EofException;
use Time\Duration;

// 1. Inheritance hierarchy
var_dump(is_subclass_of(EofException::class, TerminalException::class));
var_dump(is_subclass_of(EofException::class, \Io\IoException::class));

// 2. Open writer with no data vs closed writer
$pair = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
$readStream = $pair[0];
$writeStream = $pair[1];
$term = Terminal::fromStreams($readStream);

// Open writer with no data returns null on timeout
var_dump($term->readKey(Duration::fromMilliseconds(20)) === null);
var_dump($term->readSecret(Duration::fromMilliseconds(20)) === null);

// Close writer to signal EOF
fclose($writeStream);

// readKey throws EofException
try {
    $term->readKey(Duration::fromMilliseconds(50));
    echo "FAIL: readKey did not throw on EOF\n";
} catch (EofException $e) {
    echo "PASS: readKey threw EofException: ", $e->getMessage(), "\n";
} catch (Throwable $e) {
    echo "FAIL: readKey threw wrong exception: ", $e::class, "\n";
}

// readSecret throws EofException
try {
    $term->readSecret(Duration::fromMilliseconds(50));
    echo "FAIL: readSecret did not throw on EOF\n";
} catch (EofException $e) {
    echo "PASS: readSecret threw EofException: ", $e->getMessage(), "\n";
} catch (Throwable $e) {
    echo "FAIL: readSecret threw wrong exception: ", $e::class, "\n";
}
fclose($readStream);

// 3. Truncated UTF-8 at EOF
$pair = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fwrite($pair[1], "\xc3");
fclose($pair[1]);
$term = Terminal::fromStreams($pair[0]);

try {
    $term->readKey(Duration::fromMilliseconds(50));
    echo "FAIL: readKey did not throw on truncated UTF-8 at EOF\n";
} catch (EofException $e) {
    echo "PASS: readKey truncated UTF-8 threw EofException\n";
} catch (Throwable $e) {
    echo "FAIL: readKey truncated UTF-8 threw wrong exception: ", $e::class, "\n";
}
fclose($pair[0]);

$pair = stream_socket_pair(
    PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX,
    STREAM_SOCK_STREAM,
    PHP_OS_FAMILY === 'Windows' ? STREAM_IPPROTO_IP : 0
);
fwrite($pair[1], "\xc3");
fclose($pair[1]);
$term = Terminal::fromStreams($pair[0]);

try {
    $term->readSecret(Duration::fromMilliseconds(50));
    echo "FAIL: readSecret did not throw on truncated UTF-8 at EOF\n";
} catch (EofException $e) {
    echo "PASS: readSecret truncated UTF-8 threw EofException\n";
} catch (Throwable $e) {
    echo "FAIL: readSecret truncated UTF-8 threw wrong exception: ", $e::class, "\n";
}
fclose($pair[0]);

// 4. Secret cancellation must throw TerminalException and must NOT be EofException
foreach (["\x03\n" => "Ctrl+C", "\x04\n" => "Ctrl+D", "\x1b\n" => "Escape"] as $input => $label) {
    $fp = fopen('php://memory', 'r+');
    fwrite($fp, $input);
    rewind($fp);
    $term = Terminal::fromStreams($fp);
    try {
        $term->readSecret();
        echo "FAIL: $label did not cancel\n";
    } catch (TerminalException $e) {
        if ($e instanceof EofException) {
            echo "FAIL: $label threw EofException instead of TerminalException\n";
        } else {
            echo "PASS: $label cancelled with TerminalException\n";
        }
    } catch (Throwable $e) {
        echo "FAIL: $label threw unexpected exception: ", $e::class, "\n";
    }
    fclose($fp);
}

// 5. readLine contract: return final unterminated line, then null, never throwing EofException
$fp = fopen('php://memory', 'r+');
fwrite($fp, "unterminated line");
rewind($fp);
$term = Terminal::fromStreams($fp);

var_dump($term->readLine() === "unterminated line");
var_dump($term->readLine() === null);
fclose($fp);

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
PASS: readKey threw EofException: End of file reached on terminal input stream
PASS: readSecret threw EofException: End of file reached on terminal input stream
PASS: readKey truncated UTF-8 threw EofException
PASS: readSecret truncated UTF-8 threw EofException
PASS: Ctrl+C cancelled with TerminalException
PASS: Ctrl+D cancelled with TerminalException
PASS: Escape cancelled with TerminalException
bool(true)
bool(true)
