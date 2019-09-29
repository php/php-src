<?php

$t = 3;

function busy_wait($how_long) {
    $until = microtime(true) + $how_long;
    do {
        for ($i = 0; $i < 1000000; $i++);
    } while ($until > microtime(true));
}
