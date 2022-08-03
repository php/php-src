--TEST--
Random: Engine: serialize: user
--FILE--
<?php

final class User64 implements \Random\Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return \pack('P*', ++$this->count);
    }
}

final class User32 implements \Random\Engine
{
    private int $count = 0;

    public function generate(): string
    {
        return \pack('V', ++$this->count);
    }
}

$engines = [];
if (\PHP_INT_SIZE >= 8) {
    $engines[] = new \User64();
}
$engines[] = new \User32();

foreach ($engines as $engine) {
    for ($i = 0; $i < 1000; $i++) {
        $engine->generate();
    }
    $engine2 = unserialize(serialize($engine));
    for ($i = 0; $i < 5000; $i++) {
        if ($engine->generate() !== $engine2->generate()) {
            $className = $engine::class;
            die("failure class: {$className} i: {$i}");
        }
    }
}

die('success');
?>
--EXPECT--
success
