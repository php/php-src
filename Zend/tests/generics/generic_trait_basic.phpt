--TEST--
Generic trait: basic usage with bound type arguments
--FILE--
<?php
declare(strict_types=1);

trait Cacheable<T> {
    public T $cached;

    public function cache(T $item): void {
        $this->cached = $item;
    }

    public function getCached(): T {
        return $this->cached;
    }
}

class UserCache {
    use Cacheable<string>;
}

$uc = new UserCache();
$uc->cache("hello");
echo $uc->getCached() . "\n";

// Wrong type on method arg should throw TypeError
try {
    $uc->cache(42);
} catch (TypeError $e) {
    echo "method: caught\n";
}

// Wrong type on property should throw TypeError
try {
    $uc->cached = 42;
} catch (TypeError $e) {
    echo "property: caught\n";
}

echo "OK\n";
?>
--EXPECT--
hello
method: caught
property: caught
OK
