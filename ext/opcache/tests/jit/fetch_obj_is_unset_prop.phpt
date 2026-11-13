--TEST--
FETCH_OBJ_IS on a declared property removed by unset() must reach __isset()/__get()
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit=tracing
opcache.jit_buffer_size=16M
opcache.jit_hot_func=2
--FILE--
<?php

class Store {
    public array $marks = [];
}

class Holder {
    public static Store $store;
    public array $marks = [];

    public function __construct() {
        unset($this->marks);
    }

    public function &__get(string $name) {
        return self::$store->$name;
    }

    public function __isset(string $name): bool {
        return isset(self::$store->$name);
    }

    public function mark(string $key): void {
        $this->marks[$key] = true;
    }

    public function has(string $key): bool {
        return isset($this->marks[$key]);
    }
}

Holder::$store = new Store();
$holder = new Holder();

for ($n = 0; $n < 10; $n++) {
    $key = "k{$n}";
    $holder->mark($key);
    var_dump($holder->has($key));
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
