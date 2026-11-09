--TEST--
Io\Terminal\Terminal::enableRawMode() warns when the lease token is discarded
--FILE--
<?php

use Io\Terminal\Terminal;

$terminal = Terminal::fromStreams(fopen('php://memory', 'r+'));

$terminal->enableRawMode();
(void) $terminal->enableRawMode();

$token = $terminal->enableRawMode();
$terminal->restoreMode($token);
echo "Done\n";

?>
--EXPECTF--
Warning: The return value of method Io\Terminal\Terminal::enableRawMode() should either be used or intentionally ignored by casting it as (void), as the returned token must stay alive to keep the raw-mode lease active in %s on line %d
Done
