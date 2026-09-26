--TEST--
Io\Ring\Engine: an orphaned Read or Write never touches the caller's buffer after the call returned
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
final class GiveUp implements Io\Hooks\Hooks
{
    public int $delay = 0;
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Operation $op): void {}
    public function remove(Io\Operation $op): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        // Spin, so that the backend is busy with the op when the frame goes
        $until = hrtime(true) + $this->delay;
        while (hrtime(true) < $until);
        throw new RuntimeException("gave up");
    }
}

// The stream stays frozen until the orphaned op settled
function settle(Io\Ring\Engine $ring): void
{
    if ($ring->countPending()) {
        $ring->waitCompletions();
    }
}

$size = 1500000;
$src = __DIR__ . '/ring-orphan-caller-buffer.src';
$dst = __DIR__ . '/ring-orphan-caller-buffer.dst';
file_put_contents($src, str_repeat('A', $size));
$ring = new Io\Ring\Engine();
$provider = new GiveUp($ring);
$read_changed = 0;
$write_foreign = 0;

for ($i = 0; $i < 100; $i++) {
    $provider->delay = ($i % 50) * 10000;

    // An unbuffered read lands in a string the failed fread() frees
    $fp = fopen($src, 'r');
    stream_set_read_buffer($fp, 0);
    Io\Hooks\set_hooks($provider);
    try {
        @fread($fp, $size);
    } catch (RuntimeException $e) {
    }
    Io\Hooks\set_hooks(null);
    $victim = str_repeat('.', $size - 32);
    $until = hrtime(true) + 2000000;
    while (hrtime(true) < $until);
    $read_changed += strpos($victim, 'A') !== false;
    unset($victim);
    settle($ring);
    fclose($fp);

    // A write sends from a string the failed fwrite() frees
    $fp = fopen($dst, 'w');
    Io\Hooks\set_hooks($provider);
    try {
        @fwrite($fp, str_repeat('B', $size));
    } catch (RuntimeException $e) {
    }
    Io\Hooks\set_hooks(null);
    $victim = str_repeat('.', $size - 32);
    $until = hrtime(true) + 2000000;
    while (hrtime(true) < $until);
    unset($victim);
    settle($ring);
    fclose($fp);
    $write_foreign += trim(file_get_contents($dst), 'B') !== '';
}

var_dump($read_changed, $write_foreign);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/ring-orphan-caller-buffer.src');
@unlink(__DIR__ . '/ring-orphan-caller-buffer.dst');
?>
--EXPECT--
int(0)
int(0)
