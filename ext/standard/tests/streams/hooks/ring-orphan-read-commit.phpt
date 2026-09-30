--TEST--
Io\Ring\Engine: bytes an orphaned read took are the stream's next ones, or the stream is broken
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
final class GiveUp implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring, private $peer) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $this->ring->submit($op);
        // The data arrives once the op is in flight, and the backend takes it before the frame goes
        Io\Hooks\set_hooks(null);
        if ($this->peer) {
            fwrite($this->peer, "hello");
        }
        $until = hrtime(true) + 20000000;
        while (hrtime(true) < $until);
        throw new RuntimeException("gave up on " . $op::class);
    }
}

function orphaned_read(Io\Ring\Engine $ring, $fp, $peer, int $len): void
{
    Io\Hooks\set_hooks(new GiveUp($ring, $peer));
    try {
        @fread($fp, $len);
    } catch (RuntimeException $e) {
        echo $e->getMessage(), "\n";
    }
    Io\Hooks\set_hooks(null);
    // The stream stays frozen until the orphan settled
    while ($ring->countPending()) {
        $ring->waitCompletions();
    }
}

$ring = new Io\Ring\Engine();

echo "-- buffered socket --\n";
[$r, $w] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_timeout($r, 1);
orphaned_read($ring, $r, $w, 5);
fwrite($w, " world");
var_dump(fread($r, 11));

echo "-- unbuffered socket --\n";
[$r, $w] = stream_socket_pair(STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
stream_set_timeout($r, 1);
stream_set_read_buffer($r, 0);
orphaned_read($ring, $r, $w, 5);
var_dump(fread($r, 5), feof($r));

echo "-- buffered file --\n";
$file = __DIR__ . '/ring-orphan-read-commit.txt';
file_put_contents($file, "hello world");
$fp = fopen($file, 'r');
orphaned_read($ring, $fp, null, 5);
var_dump(fread($fp, 11), ftell($fp));
fclose($fp);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/ring-orphan-read-commit.txt');
?>
--EXPECT--
-- buffered socket --
gave up on Io\Operation\Recv
string(11) "hello world"
-- unbuffered socket --
gave up on Io\Operation\Recv
bool(false)
bool(true)
-- buffered file --
gave up on Io\Operation\Read
string(11) "hello world"
int(11)
