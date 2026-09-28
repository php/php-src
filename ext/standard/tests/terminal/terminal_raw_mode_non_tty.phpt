--TEST--
Io\Terminal\Terminal: enableRawMode returns false for non-terminal streams
--FILE--
<?php

use Io\Terminal\Terminal;

$fp = fopen('php://temp', 'r+');
$terminal = Terminal::fromStreams($fp);

var_dump($terminal->enableRawMode());
var_dump($terminal->restoreMode());

fclose($fp);
?>
--EXPECT--
bool(false)
bool(false)
