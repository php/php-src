<?php

for ($i = 0; $i < 50_000; $i++) {
    $uuid = Uuid\UuidV7::generate();
}

var_dump($uuid);
