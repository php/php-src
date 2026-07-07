--TEST--
Pipe assign operator with property hooks
--FILE--
<?php

class WithHooks {
    public string $name = 'hello' {
        set(string $value) {
            $this->name = $value;
            echo "set hook called with {$value}\n";
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

?>
--EXPECT--
get hook called
set hook called with HELLO
get hook called
HELLO
set hook called with world
get hook called
set hook called with WORLD
get hook called
WORLD
get hook called
WORLD
