--TEST--
Io\Terminal\Terminal: final class boundary, dependency injection, and testing with streams
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\ModeToken;
use Io\Terminal\Terminal;
use Io\Terminal\TerminalSize;
use Time\Duration;

// Verify final class design (no interfaces)
$rcTerm = new ReflectionClass(Terminal::class);
var_dump($rcTerm->isFinal());
var_dump($rcTerm->isInterface());

$rcToken = new ReflectionClass(ModeToken::class);
var_dump($rcToken->isFinal());
var_dump($rcToken->isInterface());

// Consumer accepts Terminal directly via dependency injection
function readUserCommand(Terminal $term): string
{
    $token = $term->enableRawMode();
    try {
        $key = $term->readKey(Duration::fromSeconds(1));
        if ($key instanceof Key) {
            return "key:" . $key->name;
        }
        return "char:" . ($key ?? "none");
    } finally {
        $term->restoreMode($token);
    }
}

// Testing with php://memory stream in pure PHP (Larry & Tim testing approach)
$in = fopen('php://memory', 'w+');
fwrite($in, "[A"); // Key::Up
rewind($in);

$term = Terminal::fromStreams($in);
echo readUserCommand($term), PHP_EOL;

// Non-tty raw mode token is valid ModeToken and restores cleanly
$memStream = fopen('php://memory', 'w+');
$memTerm = Terminal::fromStreams($memStream);
$token = $memTerm->enableRawMode();
var_dump($token instanceof ModeToken);
$memTerm->restoreMode($token);
echo "restored successfully
";

// Reusing consumed token throws ValueError
try {
    $memTerm->restoreMode($token);
    echo "FAIL: accepted consumed token
";
} catch (ValueError $e) {
    echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
key:Up
bool(true)
restored successfully
Caught: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
