--TEST--
Value class properties reject direct and indirect modification and references
--FILE--
<?php
value class State {
    public function __construct(public int $number, public array $items) {}

    public function change(): void { $this->number = 2; }
}
function byReference(&$value): void {}

$state = new State(1, [10]);
$operations = [
    function () use ($state) { $state->number = 2; },
    function () use ($state) { $state->number += 1; },
    function () use ($state) { $state->number++; },
    function () use ($state) { $state->change(); },
    function () use ($state) { $reference =& $state->number; },
    function () use ($state) { $number = 3; $state->number =& $number; },
    function () use ($state) { byReference($state->number); },
    function () use ($state) { $state->items[] = 20; },
    function () use ($state) { $state->items[0] = 20; },
    function () use ($state) { unset($state->number); },
    function () use ($state) { (new ReflectionProperty(State::class, 'number'))->setValue($state, 2); },
];
foreach ($operations as $operation) {
    try {
        $operation();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
}
var_dump($state->number, $state->items);
?>
--EXPECT--
Cannot modify readonly property State::$number
Cannot modify readonly property State::$number
Cannot modify readonly property State::$number
Cannot modify readonly property State::$number
Cannot indirectly modify readonly property State::$number
Cannot indirectly modify readonly property State::$number
Cannot indirectly modify readonly property State::$number
Cannot indirectly modify readonly property State::$items
Cannot indirectly modify readonly property State::$items
Cannot unset readonly property State::$number
Cannot modify readonly property State::$number
int(1)
array(1) {
  [0]=>
  int(10)
}
