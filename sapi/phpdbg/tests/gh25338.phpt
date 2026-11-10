--TEST--
GH-25338 (phpdbg opcode listing does not include property hooks)
--SKIPIF--
<?php
if (!getenv('TEST_PHPDBG_EXECUTABLE')) {
    die('skip phpdbg is not available');
}
?>
--FILE--
<?php
$script = tempnam(sys_get_temp_dir(), 'phpdbg_gh25338_');
register_shutdown_function(static function () use ($script) {
    if (file_exists($script)) {
        unlink($script);
    }
});
file_put_contents($script, <<<'PHP'
<?php
final class Temperature
{
    public float $celsius = 0.0 {
        set => $value;
    }

    public float $fahrenheit {
        get => $this->celsius * 9 / 5 + 32;
    }

    public int $camelCase {
        get => 42;
    }

    public function reset(): void
    {
        $this->celsius = 0.0;
    }
}
PHP);

$phpdbg = getenv('TEST_PHPDBG_EXECUTABLE_ESCAPED');
$all = shell_exec($phpdbg . ' -qn ' . escapeshellarg('-p*') . ' ' . escapeshellarg($script) . ' 2>&1');
var_dump(str_contains($all, '4 methods: reset, $celsius::set, $fahrenheit::get, $camelCase::get'));
var_dump(str_contains($all, 'Temperature::$celsius::set:'));
var_dump(str_contains($all, 'Temperature::$fahrenheit::get:'));
var_dump(str_contains($all, 'Temperature::$camelCase::get:'));

$hook = shell_exec(
    $phpdbg . ' -qn ' . escapeshellarg('-p=Temperature::$camelCase::get') . ' ' . escapeshellarg($script) . ' 2>&1'
);
var_dump(str_contains($hook, 'Temperature::$camelCase::get:'));

unlink($script);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
