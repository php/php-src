--TEST--
IO hooks: set_hooks() from getCapabilities() and add() is refused, from run() it is safe
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

final class Installed extends Base
{
    public function __destruct() { echo "Installed destroyed\n"; }
}

final class CapsReplaces extends Base
{
    public function getCapabilities(): array
    {
        try {
            Io\Hooks\set_hooks(null);
        } catch (Error $e) {
            echo $e->getMessage(), "\n";
        }
        return [];
    }
}

final class RunDrops extends Base
{
    public string $state = '';

    public function run(Io\Operation $op): Io\Completion
    {
        Io\Hooks\set_hooks(null);
        $this->state = str_repeat('x', 3);
        echo "run still has its object: {$this->state}\n";
        return $op->complete(Io\CompletionStatus::Unsupported);
    }

    public function __destruct() { echo "RunDrops destroyed\n"; }
}

final class AddReplaces extends Base
{
    private bool $reported = false;

    public function add(Io\Registration $registration): void
    {
        try {
            Io\Hooks\set_hooks(new Base());
        } catch (Error $e) {
            /* libcurl may report the socket more than once for one connect */
            if (!$this->reported) {
                echo $e->getMessage(), "\n";
                $this->reported = true;
            }
        }
    }
}

Io\Hooks\set_hooks(new Installed());
$previous = Io\Hooks\set_hooks(new CapsReplaces());
var_dump($previous instanceof Installed);
unset($previous);

Io\Hooks\set_hooks(new RunDrops());
usleep(1);
var_dump(Io\Hooks\get_hooks());

Io\Hooks\set_hooks(new AddReplaces());
$ch = curl_init("http://127.0.0.1:1/");
curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
curl_setopt($ch, CURLOPT_CONNECTTIMEOUT_MS, 200);
var_dump(curl_exec($ch));
var_dump(Io\Hooks\get_hooks() instanceof AddReplaces);
Io\Hooks\set_hooks(null);
echo "done\n";
?>
--EXPECT--
Io\Hooks\set_hooks() cannot be called from getCapabilities(), add(), remove() or a provider destructor
bool(true)
Installed destroyed
run still has its object: xxx
RunDrops destroyed
NULL
Io\Hooks\set_hooks() cannot be called from getCapabilities(), add(), remove() or a provider destructor
bool(false)
bool(true)
done
