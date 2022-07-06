--TEST--
func_num_args() outside of a function declaration
--FILE--
<?php

try {
    func_num_args();
} catch (Error $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
func_num_args() must be called from a function context
