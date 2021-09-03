<?php

$corpusDir = __DIR__ . '/corpus/unserializehash';
@mkdir($corpusDir);

foreach (hash_algos() as $algo) {
    $ctx = hash_init($algo);
    $algx = preg_replace('/[^-_a-zA-Z0-9]/', '_', $algo);
    try {
        $serialized = serialize($ctx);
    } catch (Exception $e) {
        echo "Hash algorithm $algo could not be serialized.\n";
        continue;
    }
    file_put_contents($corpusDir . '/' . $algx, "x|" . $serialized);
}
