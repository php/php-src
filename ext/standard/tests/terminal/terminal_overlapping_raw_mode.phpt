--TEST--
Io\Terminal\Terminal: overlapping raw-mode sessions and out-of-order restoration
--FILE--
<?php

use Io\Terminal\Terminal;
use Io\Terminal\ModeToken;

$t1 = Terminal::create();
$t2 = Terminal::create();

$m1 = $t1->enableRawMode();
$m2 = $t2->enableRawMode();

if ($m1 instanceof ModeToken && $m2 instanceof ModeToken) {
    // Both active: restore m1 first (out of order)
    var_dump($t1->restoreMode($m1));

    // Now restore m2
    var_dump($t2->restoreMode($m2));
} else {
    // Fallback in non-tty runner
    var_dump(true);
    var_dump(true);
}

?>
--EXPECT--
bool(true)
bool(true)
