--TEST--
Inc/dec on undefined variable: warning converted to exception
--FILE--
<?php

set_error_handler(function($severity, $m) {
    throw new Exception($m, $severity);
});

unset($x);
try {
    $x++;
} catch (\Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    if (compact('x') == []) { echo("UNDEF\n"); } else { var_dump($x); }
}
unset($x);
try {
    $x--;
} catch (\Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    if (compact('x') == []) { echo("UNDEF\n"); } else { var_dump($x); }
}
unset($x);
try {
    ++$x;
} catch (\Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    if (compact('x') == []) { echo("UNDEF\n"); } else { var_dump($x); }
}
unset($x);
try {
    --$x;
} catch (\Exception $e) {
    echo $e->getMessage(), PHP_EOL;
    if (compact('x') == []) { echo("UNDEF\n"); } else { var_dump($x); }
}
unset($x);
?>
--EXPECT--
Undefined variable $x
NULL
Undefined variable $x
NULL
Undefined variable $x
NULL
Undefined variable $x
NULL
