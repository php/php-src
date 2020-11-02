--TEST--
Hash: serialization formats
--FILE--
<?php

function test_serialization($serial, $hash, $algo) {
    try {
        $ctx = unserialize(base64_decode($serial));
        hash_update($ctx, "Can’t tell if this is true or dream");
        $hash2 = hash_final($ctx);
        if ($hash !== $hash2) {
            echo "$algo: unexpected hash $hash2 for serialization {$serial}\n";
        }
    } catch (Throwable $e) {
        echo "$algo: problem with serialization {$serial}\n";
        echo "  ", $e->getMessage(), "\n", $e->getTraceAsString();
    }
}

foreach (hash_algos() as $algo) {
    $hash = hash($algo, "I can't remember anythingCan’t tell if this is true or dream");

    $ctx = hash_init($algo);
    hash_update($ctx, "I can't remember anything");
    $serial = base64_encode(serialize($ctx));

    test_serialization($serial, $hash, $algo);
}

echo "Done\n";
?>
--EXPECT--
Done
