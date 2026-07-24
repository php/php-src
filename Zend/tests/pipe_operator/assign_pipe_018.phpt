--TEST--
Pipe assign operator with property hooks
--FILE--
<?php

class WithHooks {
    public string $name = 'hello' {
        set(string $value) {
            $this->name = $value;
            echo "set hook called\n";
        }
        get {
            echo "get hook called\n";
            return $this->name;
        }
    }

    public string $upper {
        get => strtoupper($this->name);
    }
}

$obj = new WithHooks();
$obj->name |>= strtoupper(...);
echo $obj->name, "\n";

$obj->name = 'world';
$obj->name |>= strtoupper(...) |> trim(...);
echo $obj->name, "\n";

echo $obj->upper, "\n";

class WithReadonly {
    public function __construct(
        public readonly string $value = 'hello',
    ) {}
}

$ro = new WithReadonly();
try {
    $ro->value |>= strtoupper(...);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
get hook called
set hook called
get hook called
HELLO
set hook called
get hook called
set hook called
get hook called
WORLD
get hook called
WORLD
Cannot modify readonly property WithReadonly::$value
