--TEST--
FETCH_OBJ_IS on a lazy object must initialize it instead of reading the IS_UNDEF slot
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
--FILE--
<?php

class Marks {
    public $list = null;
}

function probe($holder): bool {
    return isset($holder->list['hot']);
}

$plain = new Marks();
$reflection = new ReflectionClass(Marks::class);
$inits = 0;

/* The trace is recorded on plain instances, whose property holds NULL, so it
   guards the result of FETCH_OBJ_IS against NULL. Every slot of a lazy ghost is
   IS_UNDEF as well, and reading one has to run the initializer. */
$holders = array_fill(0, 64, $plain);

for ($n = 64; $n < 96; $n++) {
    $holders[$n] = $reflection->newLazyGhost(function ($holder) use (&$inits) {
        $inits++;
        $holder->list = ['hot' => true];
    });
}

$hits = 0;
for ($n = 0; $n < 96; $n++) {
    $hits += probe($holders[$n]) ? 1 : 0;
}

var_dump($hits, $inits, $reflection->isUninitializedLazyObject($holders[95]));

?>
--EXPECT--
int(32)
int(32)
bool(false)
