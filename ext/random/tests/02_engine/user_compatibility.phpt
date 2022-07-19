--TEST--
Random: Engine: User: compatibility
--FILE--
<?php

$native_engine = new \Random\Engine\Mt19937(1234);
$user_engine = new class () implements \Random\Engine {
    public function __construct(private $engine = new \Random\Engine\Mt19937(1234))
    {
    }

    public function generate(): string
    {
        return $this->engine->generate();
    }
};

for ($i = 0; $i < 1000; $i++) {
    if ($native_engine->generate() !== $user_engine->generate()) {
        die('failure Mt19937');
    }
}

$native_engine = new \Random\Engine\PcgOneseq128XslRr64(1234);
$user_engine = new class () implements \Random\Engine {
    public function __construct(private $engine = new \Random\Engine\PcgOneseq128XslRr64(1234))
    {
    }

    public function generate(): string
    {
        return $this->engine->generate();
    }
};

for ($i = 0; $i < 1000; $i++) {
    if ($native_engine->generate() !== $user_engine->generate()) {
        die('failure PcgOneseq128XslRr64');
    }
}

$native_engine = new \Random\Engine\Xoshiro256StarStar(1234);
$user_engine = new class () implements \Random\Engine {
    public function __construct(private $engine = new \Random\Engine\Xoshiro256StarStar(1234))
    {
    }

    public function generate(): string
    {
        return $this->engine->generate();
    }
};

for ($i = 0; $i < 1000; $i++) {
    if ($native_engine->generate() !== $user_engine->generate()) {
        die('failure Xoshiro256StarStar');
    }
}

die('success');
?>
--EXPECT--
success
