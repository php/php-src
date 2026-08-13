--TEST--
A NULL result guard on FETCH_OBJ_IS must not admit the IS_UNDEF slot of an unset() property
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

class Slot {
    public $marks;
}

class Holder {
    public static Slot $slot;
    public array $marks = [];

    public function __construct() {
        unset($this->marks);
    }

    public function &__get(string $name) {
        return self::$slot->$name;
    }

    public function __isset(string $name): bool {
        return isset(self::$slot->$name);
    }

    public function has(string $key): bool {
        return isset($this->marks[$key]);
    }
}

Holder::$slot = new Slot();
$holder = new Holder();

/* The trace is recorded while __isset() answers false, so it guards the result
   of FETCH_OBJ_IS against NULL. The slot of the unset() property is IS_UNDEF,
   and admitting it as NULL would answer the second half of the loop from the
   trace without ever calling __isset()/__get(). */
$hits = 0;
for ($n = 0; $n < 96; $n++) {
    if ($n === 64) {
        Holder::$slot->marks = ['hot' => true];
    }
    $hits += $holder->has('hot') ? 1 : 0;
}

var_dump($hits);

?>
--EXPECT--
int(32)
