--TEST--
Variance: full happy-path showing every legal combination
--FILE--
<?php
class Animal {}
class Dog extends Animal {}

interface Producer<out T> { public function produce(): T; }
interface Consumer<in T> { public function consume(T $x): void; }
interface Inv<U> { public function rw(U $x): U; }

class CoProducer<out T> {
    public readonly T $tag;
    public function __construct(T $tag) { $this->tag = $tag; }
    public function get(): T {}
    public function makeProducer(): Producer<T> {}
    public function makeConsumerOfConsumers(): Consumer<Consumer<T>> {}
}

class ReadOnlyHooked<out T> {
    private T $backing;
    public function __construct(T $v) { $this->backing = $v; }
    public T $val { get => $this->backing; }
}

class CoConsumer<in T> {
    public function take(T $x): void {}
    public function takeMany(T ...$xs): void {}
    public function makeConsumer(): Consumer<T> {}
    public function consumerOfProducers(Producer<T> $p): void {}
}

class Holder<T> {
    public T $val;
    public function __construct(T $v) { $this->val = $v; }
    public function get(): T { return $this->val; }
    public function set(T $v): void { $this->val = $v; }
    public T $hooked {
        get => $this->val;
        set(T $v) { $this->val = $v; }
    }
}

interface Source<out T> { public function fetch(): T; }
class StringSource implements Source<string> {
    public function fetch(): string { return "ok"; }
}

class CovariantChain<out T> implements Source<T> {
    public function __construct(public readonly T $value) {}
    public function fetch(): T { return $this->value; }
}

class MethodLevel<out T> {
    public function map<U>(U $key): T {}
}

trait TraitProducer<out U> {
    public function produceU(): U {}
}
class UsesProducerTrait {
    use TraitProducer<int>;
    public function produceU(): int { return 7; }
}

$h = new Holder::<int>(42);
$h->set(7);
echo $h->get(), "\n";

$rh = new ReadOnlyHooked::<string>("hi");
echo $rh->val, "\n";

$cp = new CoProducer::<Dog>(new Dog());
echo get_class($cp->tag), "\n";

echo (new UsesProducerTrait)->produceU(), "\n";
echo (new StringSource)->fetch(), "\n";

echo "all variance combinations OK\n";
?>
--EXPECT--
7
hi
Dog
7
ok
all variance combinations OK
