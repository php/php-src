--TEST--
Value classes inherit readonly defaults and delayed initialization
--FILE--
<?php
value class State {
    public int $default = 42;
    public string $late;
    public function initialize(string $value): void { $this->late = $value; }
}
$state = new State;
var_dump($state->default, isset($state->late));
$state->initialize('first');
var_dump($state->late);
try {
    $state->initialize('second');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $state->default = 43;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
int(42)
bool(false)
string(5) "first"
Cannot modify readonly property State::$late
Cannot modify readonly property State::$default
