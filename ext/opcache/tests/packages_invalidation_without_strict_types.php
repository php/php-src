<?php

package_declare([
    "name" => "pkg",
    "declares" => ["strict_types" => 0],
]);

require __DIR__ . '/packages_invalidation_use_pkg.php';
echo "Misses: ", opcache_get_status()["opcache_statistics"]["misses"] ?? "-", "\n";
