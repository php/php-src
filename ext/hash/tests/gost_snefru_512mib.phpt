--TEST--
Hash: gost and snefru with 512 MiB of input (bit counter carry)
--SKIPIF--
<?php
if (!getenv('RUN_RESOURCE_HEAVY_TESTS')) die('skip resource-heavy test');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
if (getenv('SKIP_MSAN')) die('skip msan too slow');
?>
--INI--
memory_limit=-1
--FILE--
<?php
/* 2^29 bytes are 2^32 bits, so the low word of the bit counter wraps. */
$chunk = str_repeat('a', 1 << 20);
foreach (['gost', 'snefru'] as $algo) {
    $ctx = hash_init($algo);
    for ($i = 0; $i < 512; $i++) {
        hash_update($ctx, $chunk);
    }
    echo $algo, ': ', hash_final($ctx), "\n";
}
unset($chunk);

/* A single update of 2^29 bytes. */
echo 'snefru: ', hash('snefru', str_repeat('a', 1 << 29)), "\n";
?>
--EXPECT--
gost: 6c30acc180f544a79c22fa763593a812aec721dd61658b1a5e9a6f3a7955146d
snefru: 0842a8e735ab08a30149e7e6e34833f365291978ee68c37a5135253b70afa486
snefru: 0842a8e735ab08a30149e7e6e34833f365291978ee68c37a5135253b70afa486
