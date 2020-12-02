--TEST--
Use of @ in try catch must not fail when a diagnostic is emitted
--FILE--
<?php

try {
    $var = @$non_existent;
} catch (\Throwable $e) {
    echo $e::class, \PHP_EOL;
}

var_dump($var);

echo "Done\n";
?>
--EXPECT--
NULL
Done
