--TEST--
Io\Terminal\SystemTerminal: enableRawMode throws TerminalException for non-terminal streams
--FILE--
<?php

use Io\Terminal\SystemTerminal;

$fp = fopen('php://temp', 'r+');
$terminal = SystemTerminal::fromStreams($fp);

try {
    $terminal->enableRawMode();
    echo "FAIL: enableRawMode on non-terminal did not throw\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}
var_dump($terminal->restoreMode());

fclose($fp);
?>
--EXPECT--
Io\Terminal\TerminalException: Failed to enable terminal raw mode
bool(false)
