--TEST--
Io\Ring\Engine: freeing the ring while a fiber is suspended in an operation on it
--SKIPIF--
<?php
if (!class_exists(Io\Ring\Engine::class)) die("skip Io\\Ring\\Engine not available");
?>
--FILE--
<?php
$GLOBALS['ring'] = new Io\Ring\Engine();
Io\Hooks\set_hooks(new class implements Io\Hooks\Hooks {
    public function getCapabilities(): array { return $GLOBALS['ring']->getHookCapabilities(); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
    public function run(Io\Operation $op): Io\Completion {
        $c = $GLOBALS['ring']->submit($op, "data");
        $GLOBALS['kept'] = $op;
        return $c ?? Fiber::suspend();
    }
});

[$a, $b] = stream_socket_pair(PHP_OS_FAMILY === 'Windows' ? STREAM_PF_INET : STREAM_PF_UNIX, STREAM_SOCK_STREAM, 0);
$f = new Fiber(function () use ($a) {
    try {
        fread($a, 10);
    } finally {
        echo "fiber unwinding\n";
    }
});
$f->start();
echo "suspended\n";

// The ring cancels and drains the operation; the fiber's frame still has it
unset($GLOBALS['ring']);
echo "ring freed\n";
$reuse = str_repeat("z", 200);
var_dump($kept->isValid());

$f = null;
echo "fiber destroyed\n";
var_dump($kept->isValid());

Io\Hooks\set_hooks(null);
fwrite($b, "x");
var_dump(fread($a, 10));
?>
--EXPECT--
suspended
ring freed
bool(true)
fiber unwinding
fiber destroyed
bool(false)
string(1) "x"
