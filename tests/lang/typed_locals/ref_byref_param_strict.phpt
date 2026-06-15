--TEST--
Typed local variables: by-reference parameter is enforced under strict_types
--FILE--
<?php
declare(strict_types=1);

function m(&$r) { $r = "s"; }

int $x = 1;
try {
    m($x);
} catch (\TypeError $e) {
    echo "TypeError\n";
}
var_dump($x);
?>
--EXPECT--
TypeError
int(1)
