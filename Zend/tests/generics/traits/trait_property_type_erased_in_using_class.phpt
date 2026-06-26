--TEST--
Traits: a trait property typed with the using-class type argument erases to its base class
--FILE--
<?php
class State<T> {}

trait Side<TS> {
    protected TS $state;
    public function __construct(TS $s) { $this->state = $s; }
    public function state(): TS { return $this->state; }
}

final class Receiver<T> {
    use Side<State<T>>;
}

$r = new Receiver::<int>(new State::<int>());
\var_dump($r->state() instanceof State);

echo (string) (new ReflectionProperty(Receiver::class, 'state'))->getType(), "\n";
echo (string) (new ReflectionMethod(Receiver::class, '__construct'))->getParameters()[0]->getType(), "\n";
echo (string) (new ReflectionMethod(Receiver::class, 'state'))->getReturnType(), "\n";

try {
    new Receiver::<int>(new stdClass());
} catch (\TypeError $e) {
    echo "rejected: ", $e->getMessage(), "\n";
}
echo "ok\n";
?>
--EXPECTF--
bool(true)
State
State
State
rejected: Receiver::__construct(): Argument #1 ($s) must be of type State, stdClass given, called in %s on line %d
ok
