--TEST--
Io\Terminal\Terminal: getDimensions returns null for non-terminal streams without environment fallback
--ENV--
COLUMNS=100
LINES=30
--FILE--
<?php

use Io\Terminal\Terminal;

$fp = fopen('php://temp', 'r+');
$terminal = Terminal::fromStreams($fp);

var_dump($terminal->getDimensions());

?>
--EXPECT--
NULL
