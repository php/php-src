--TEST--
Io\Terminal\SystemTerminal: getSize returns null for non-terminal streams without environment fallback
--ENV--
COLUMNS=100
LINES=30
--FILE--
<?php

use Io\Terminal\SystemTerminal;

$fp = fopen('php://temp', 'r+');
$terminal = SystemTerminal::fromStreams($fp);

var_dump($terminal->getSize());

?>
--EXPECT--
NULL
