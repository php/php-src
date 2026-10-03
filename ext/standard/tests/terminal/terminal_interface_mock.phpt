--TEST--
Io\Terminal\Terminal: mockable terminal boundary and foreign token rejection
--FILE--
<?php

use Io\Terminal\Key;
use Io\Terminal\ModeToken;
use Io\Terminal\SystemModeToken;
use Io\Terminal\Terminal;
use Io\Terminal\SystemTerminal;
use Io\Terminal\TerminalSize;
use Time\Duration;

// Verify interface hierarchy
$rcTerm = new ReflectionClass(SystemTerminal::class);
var_dump($rcTerm->implementsInterface(Terminal::class));

$rcToken = new ReflectionClass(SystemModeToken::class);
var_dump($rcToken->implementsInterface(ModeToken::class));

// Application-level fake
class FakeModeToken implements ModeToken {}

class FakeTerminal implements Terminal
{
    public bool $isRaw = false;

    public function getSize(): ?TerminalSize
    {
        return new TerminalSize(120, 40);
    }

    public function enableRawMode(): ModeToken
    {
        $this->isRaw = true;
        return new FakeModeToken();
    }

    public function restoreMode(?ModeToken $mode = null): bool
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

    public function readLine(): ?string
    {
        return "mocked-line";
    }

    public function readSecret(?Duration $timeout = null): ?string
    {
        return "mocked-secret";
    }
}

// Application service consuming Terminal
function promptPassword(Terminal $term): string
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

// Native restoreMode rejects foreign ModeToken implementations
$fp = fopen('php://temp', 'r+');
$native = SystemTerminal::fromStreams($fp);

try {
    $native->restoreMode(new FakeModeToken());
    echo "FAIL: native accepted foreign ModeToken\n";
} catch (ValueError $e) {
    echo "Caught: ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
bool(true)
bool(true)
size=120x40 secret=mocked-secret
bool(false)
Caught: Io\Terminal\SystemTerminal::restoreMode(): Argument #1 ($mode) must be an active terminal mode token belonging to this terminal
