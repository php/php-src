<?php

require_once __DIR__ . "/../../app/ramsey_uuid/vendor/autoload.php";

for ($i = 0; $i < 50_000; $i++) {
    $uuid = Ramsey\Uuid\Uuid::uuid7();
}

var_dump($uuid);
