--TEST--
Io\Terminal\TerminalInterface: mockable terminal boundary and foreign token rejection
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\ModeToken;
use Io\Terminal\ModeTokenInterface;
use Io\Terminal\Terminal;
use Io\Terminal\TerminalInterface;
use Io\Terminal\TerminalSize;
use Time\Duration;

// Verify interface hierarchy
$rcTerm = new ReflectionClass(Terminal::class);
var_dump($rcTerm->implementsInterface(TerminalInterface::class));

$rcToken = new ReflectionClass(ModeToken::class);
var_dump($rcToken->implementsInterface(ModeTokenInterface::class));

// Application-level fake
class FakeModeToken implements ModeTokenInterface {}

class FakeTerminal implements TerminalInterface
{
    public bool $isRaw = false;

    public function getSize(): ?TerminalSize
    {
        return new TerminalSize(120, 40);
    }

    public function enableRawMode(): ModeTokenInterface
    {
        $this->isRaw = true;
        return new FakeModeToken();
    }

    public function restoreMode(?ModeTokenInterface $mode = null): bool
    {
        $this->isRaw = false;
        return true;
    }

    public function readKey(
        ?Duration $timeout = null,
        ?Duration $sequenceTimeout = null,
    ): Key|string|null {
        return Key::Up;
    }

    public function readSecret(?Duration $timeout = null): ?string
    {
        return "mocked-secret";
    }
}

// Application service consuming TerminalInterface
function promptPassword(TerminalInterface $term): string
{
    $size = $term->getSize();
    $token = $term->enableRawMode();
    try {
        $secret = $term->readSecret(Duration::fromSeconds(5));
        return sprintf("size=%dx%d secret=%s", $size->cols, $size->rows, $secret ?? 'none');
    } finally {
        $term->restoreMode($token);
    }
}

$fake = new FakeTerminal();
echo promptPassword($fake), PHP_EOL;
var_dump($fake->isRaw);

// Native restoreMode rejects foreign ModeTokenInterface implementations
$fp = fopen('php://temp', 'r+');
$native = Terminal::fromStreams($fp);

try {
    $native->restoreMode(new FakeModeToken());
    echo "FAIL: native accepted foreign ModeTokenInterface\n";
} catch (ValueError $e) {
    echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
size=120x40 secret=mocked-secret
bool(false)
Caught: Io\Terminal\Terminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
