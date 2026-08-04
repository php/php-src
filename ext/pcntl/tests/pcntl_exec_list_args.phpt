--TEST--
pcntl_exec(): Argument array must be a list
--EXTENSIONS--
pcntl
--FILE--
<?php
try {
    pcntl_exec('cmd', ['opt' => '-n']);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: pcntl_exec(): Argument #2 ($args) must be a list array
