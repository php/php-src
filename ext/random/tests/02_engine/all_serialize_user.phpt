--TEST--
Random: Engine: Serialization of user engines must preserve the sequence
--FILE--
<?php

use Random\Engine;

final class User64 implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return pack('P*', ++$this->count);
    }
}

final class User32 implements Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return pack('V', ++$this->count);
    }
}

$engines = [];
if (PHP_INT_SIZE >= 8) {
    $engines[] = new User64();
}
$engines[] = new User32();

foreach ($engines as $engine) {
    for ($i = 0; $i < 10_000; $i++) {
        $engine->generate();
    }

    $engine2 = unserialize(serialize($engine));

    for ($i = 0; $i < 10_000; $i++) {
        if ($engine->generate() !== $engine2->generate()) {
            $className = $engine::class;

            die("failure: {$className} at {$i}");
        }
    }
}

die('success');

?>
--EXPECT--
success
