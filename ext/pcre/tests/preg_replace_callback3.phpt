--TEST--
preg_replace_callback() 3
--FILE--
<?php

try {
    preg_replace_callback(1, 2, 3);
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}

echo "Done\n";
?>
--EXPECT--
preg_replace_callback(): Argument #2 ($callback) must be a valid callback, no array or string given
Done
