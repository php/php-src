--TEST--
pcntl_exec(): Test cleanup after value that contain null bytes has been encountered for $args and $env_vars.
--EXTENSIONS--
pcntl
--FILE--
<?php
try {
    pcntl_exec('cmd', ['-n', "value\0null\0byte"]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    pcntl_exec(
        'cmd',
        ['-n'],
        ['var1' => 'value1', 'var2' => "value\0null\0byte"],
    );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    pcntl_exec(
        'cmd',
        ['-n'],
        ['var1' => 'value1', "key\0null\0byte" => "value2"],
    );
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ValueError: pcntl_exec(): Argument #2 ($args) individual argument must not contain null bytes
ValueError: pcntl_exec(): Argument #3 ($env_vars) value for environment variable must not contain null bytes
ValueError: pcntl_exec(): Argument #3 ($env_vars) name for environment variable must not contain null bytes
