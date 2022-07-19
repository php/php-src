--TEST--
Random: Engine: serialize: native
--FILE--
<?php

$engines = [];
$engines[] = new \Random\Engine\Mt19937(1234);
$engines[] = new \Random\Engine\PcgOneseq128XslRr64(1234);
$engines[] = new \Random\Engine\Xoshiro256StarStar(1234);

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
