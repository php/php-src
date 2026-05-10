--TEST--
Trait methods: alias-renamed method on a generic trait still carries the substituted contract
--FILE--
<?php
trait Holder<T> {
    public function take(T $v): void {}
}

class IntHolder {
    use Holder<int> {
        take as accept;
    }
}

(new IntHolder())->take(1);
(new IntHolder())->accept(2);

try {
    (new IntHolder())->take("nope");
} catch (TypeError $e) {
    echo "take: ", $e->getMessage(), "\n";
}

try {
    (new IntHolder())->accept("nope");
} catch (TypeError $e) {
    echo "accept: ", $e->getMessage(), "\n";
}
?>
--EXPECTF--
take: IntHolder::take(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
accept: IntHolder::accept(): Argument #1 ($v) must be of type int, string given, called in %s on line %d
