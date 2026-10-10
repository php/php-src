--TEST--
IO hooks: a pipe read or write the provider abandons with an exception is not a stream error
--SKIPIF--
<?php if (PHP_OS_FAMILY === 'Windows') die('skip plain-wrapper pipes are not hooked on Windows'); ?>
--FILE--
<?php
$throwing = new class implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return [Io\Hooks\Capability::DirectData]; }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        throw new RuntimeException("cancelled " . $op::class);
    }
};

function cancelled(Io\Hooks\Hooks $hooks, callable $fn): void
{
    Io\Hooks\set_hooks($hooks);
    try {
        $fn();
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    } finally {
        Io\Hooks\set_hooks(null);
    }
}

$php = getenv('TEST_PHP_EXECUTABLE');
$proc = proc_open([$php, '-n', '-r', 'echo fgets(STDIN);'], [['pipe', 'r'], ['pipe', 'w']], $pipes);
cancelled($throwing, fn () => fread($pipes[1], 10));
cancelled($throwing, fn () => fwrite($pipes[0], "lost\n"));
var_dump(feof($pipes[1]));
fwrite($pipes[0], "ping\n");
var_dump(fgets($pipes[1]));
fclose($pipes[0]);
fclose($pipes[1]);
proc_close($proc);
?>
--EXPECT--
cancelled Io\Operation\Read
cancelled Io\Operation\Write
bool(false)
string(5) "ping
"
