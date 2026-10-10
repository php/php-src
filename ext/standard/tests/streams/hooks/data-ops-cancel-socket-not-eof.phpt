--TEST--
IO hooks: a socket read the provider abandons with an exception does not set eof
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

[$r, $w] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
cancelled($throwing, fn () => fread($r, 10));
var_dump(feof($r));
fwrite($w, "x");
var_dump(fread($r, 10));

?>
--EXPECT--
cancelled Io\Operation\Recv
bool(false)
string(1) "x"
