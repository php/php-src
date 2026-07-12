--TEST--
A deferred destructor dropped in a generator is caught by that generator, not the consumer
--FILE--
<?php
class D {
    public function __construct(public int $id) {}
    public function __destruct() { throw new Exception("dtor {$this->id}"); }
}

function gen() {
    try {
        $d = new D(1);
        unset($d);
        yield 1;
        yield 2;
    } catch (Exception $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}

try {
    foreach (gen() as $v) {
        echo "consumer got $v\n";
    }
    echo "consumer done\n";
} catch (Exception $e) {
    echo "ESCAPED to consumer: ", $e->getMessage(), "\n";
}

function inner() {
    try {
        $d = new D(2);
        unset($d);
        yield 1;
    } catch (Exception $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    yield 3;
}
function outer() { yield from inner(); }

try {
    foreach (outer() as $v) {
        echo "outer got $v\n";
    }
    echo "outer done\n";
} catch (Exception $e) {
    echo "ESCAPED to outer consumer: ", $e->getMessage(), "\n";
}
echo "end\n";
?>
--EXPECT--
Exception: dtor 1
consumer done
Exception: dtor 2
outer got 3
outer done
end
