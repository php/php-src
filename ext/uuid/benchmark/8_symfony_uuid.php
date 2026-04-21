<?php

require_once __DIR__ . "/../../app/symfony_uuid/vendor/autoload.php";

for ($i = 0; $i < 50_000; $i++) {
    $uuid = new Symfony\Component\Uid\UuidV7();
}

var_dump($uuid);
