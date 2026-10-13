--TEST--
Io\Ring\Engine: an accepted descriptor 0 is a valid result
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
if (PHP_OS_FAMILY === 'Windows') die("skip descriptor numbering is POSIX");
?>
--FILE--
<?php
final class Waiting implements Io\Hooks\Hooks
{
    public function __construct(private Io\Ring\Engine $ring) {}
    public function getCapabilities(): array { return $this->ring->getSupportedHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion
    {
        $c = $this->ring->submit($op);
        while ($c === null) {
            foreach ($this->ring->waitCompletions() as $done) {
                $c = $done;
            }
        }
        return $c;
    }
}

$provider = new Waiting(new Io\Ring\Engine());
$server = stream_socket_server('tcp://127.0.0.1:0');
$client = stream_socket_client('tcp://' . stream_socket_get_name($server, false));
// The lowest free descriptor goes to the accepted connection
fclose(STDIN);
Io\Hooks\set_hooks($provider);
$conn = stream_socket_accept($server, 5);
Io\Hooks\set_hooks(null);
var_dump(is_resource($conn));
fwrite($client, "hello");
var_dump(fread($conn, 5));
?>
--EXPECT--
bool(true)
string(5) "hello"
