--TEST--
GH-11821: Exit in autoloader during class fetch should not cause UnwindExit string conversion error
--FILE--
<?php

spl_autoload_register(function ($class) {
    exit(0);
});

class A {
    use NonExistentTrait;
}

echo "Should not reach here\n";
?>
--EXPECT--
