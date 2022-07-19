--TEST--
Random: Randomizer: Compatibility: user
--FILE--
<?php

$native_randomizer = new \Random\Randomizer(new \Random\Engine\Mt19937(1234));
$user_randomizer = new \Random\Randomizer(new class () implements \Random\Engine {
    public function __construct(private $engine = new \Random\Engine\Mt19937(1234))
    {
    }

    public function generate(): string
    {
        return $this->engine->generate();
    }
});
for ($i = 0; $i < 1000; $i++) {
    $native = $native_randomizer->getInt();
    $user = $user_randomizer->getInt();
    if ($native !== $user) {
        die("failure Mt19937 i: {$i} native: {$native} user: {$user}");
    }
}

try {
    $native_randomizer = new \Random\Randomizer(new \Random\Engine\PcgOneseq128XslRr64(1234));
    $user_randomizer = new \Random\Randomizer(new class () implements \Random\Engine {
        public function __construct(private $engine = new \Random\Engine\PcgOneseq128XslRr64(1234))
        {
        }

        public function generate(): string
        {
            return $this->engine->generate();
        }
    });
    
    for ($i = 0; $i < 1000; $i++) {
        $native = $native_randomizer->getInt();
        $user = $user_randomizer->getInt();
        if ($native !== $user) {
            die("failure PcgOneseq128XslRr64 i: {$i} native: {$native} user: {$user}");
        }
    }
} catch (\RuntimeException $e) {
    if (\PHP_INT_SIZE >= 8) {
        throw $e;
    }
    if ($e->getMessage() !== 'Generated value exceeds size of int') {
        throw $e;
    }
}

try {
    $native_randomizer = new \Random\Randomizer(new \Random\Engine\Xoshiro256StarStar(1234));
    $user_randomizer = new \Random\Randomizer(new class () implements \Random\Engine {
        public function __construct(private $engine = new \Random\Engine\Xoshiro256StarStar(1234))
        {
        }

        public function generate(): string
        {
            return $this->engine->generate();
        }
    });
    
    for ($i = 0; $i < 1000; $i++) {
        $native = $native_randomizer->getInt();
        $user = $user_randomizer->getInt();
        if ($native !== $user) {
            die("failure Xoshiro256StarStar i: {$i} native: {$native} user: {$user}");
        }
    }
} catch (\RuntimeException $e) {
    if (\PHP_INT_SIZE >= 8) {
        throw $e;
    }
    if ($e->getMessage() !== 'Generated value exceeds size of int') {
        throw $e;
    }
}

die('success');
?>
--EXPECT--
success
