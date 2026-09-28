--TEST--
IO hooks: getCapabilities() and add() cannot switch fibers
--EXTENSIONS--
curl
--FILE--
<?php

class Base implements Io\Hooks\Hooks
{
    public function getCapabilities(): array { return [Io\Hooks\Capability::LevelRegistrations]; }
    public function run(Io\Operation $op): Io\Completion { return $op->complete(Io\CompletionStatus::Unsupported); }
    public function add(Io\Registration $registration): void {}
    public function remove(Io\Registration $registration): void {}
}

final class CapsSuspends extends Base
{
    public function getCapabilities(): array
    {
        Fiber::suspend();
        return [];
    }
}

final class AddSuspends extends Base
{
    public function add(Io\Registration $registration): void
    {
        try {
            Fiber::suspend();
        } catch (FiberError $e) {
            echo "add: ", $e->getMessage(), "\n";
        }
    }
}

$fiber = new Fiber(function () {
    try {
        Io\Hooks\set_hooks(new CapsSuspends());
    } catch (FiberError $e) {
        echo "getCapabilities: ", $e->getMessage(), "\n";
    }
    var_dump(Io\Hooks\get_hooks());

    Io\Hooks\set_hooks(new AddSuspends());
    $ch = curl_init("http://127.0.0.1:1/");
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 200);
    var_dump(curl_exec($ch));
    /* Not left locked */
    Io\Hooks\set_hooks(null);
    echo "uninstalled\n";
});
$fiber->start();
var_dump($fiber->isTerminated());
?>
--EXPECT--
getCapabilities: Cannot switch fibers in current execution context
NULL
add: Cannot switch fibers in current execution context
bool(false)
uninstalled
bool(true)
