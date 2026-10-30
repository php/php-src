--TEST--
GH-19860 (Nested unserialize() finalizes pending magic methods)
--FILE--
<?php
function custom_serialized(string $class, string $data): string {
    return sprintf('C:%d:"%s":%d:{%s}', strlen($class), $class, strlen($data), $data);
}

class Pending {
    public string $value;

    public function __construct(string $value) {
        $this->value = $value;
    }

    public function __unserialize(array $data): void {
        $this->value = $data['value'];
    }
}

class Legacy implements Serializable {
    public $value;

    public function __construct($value = null) {
        $this->value = $value;
    }

    public function serialize(): string {
        return serialize($this->value);
    }

    public function __serialize(): array {
        return ['value' => $this->value];
    }

    public function unserialize(string $data): void {
        $this->value = unserialize($data);
        echo $this->value->value, PHP_EOL;
    }

    public function __unserialize(array $data): void {
        $this->value = $data['value'];
    }
}

$result = unserialize(custom_serialized(Legacy::class, serialize(new Pending('nested'))));
echo $result->value->value, PHP_EOL;

$result = unserialize('a:2:{i:0;' . serialize(new Pending('outer'))
    . 'i:1;' . custom_serialized(Legacy::class, 'r:2;') . '}');
var_dump($result[0] === $result[1]->value);

class ThrowingPending {
    public function __construct(private string $label) {}

    public function __serialize(): array {
        return ['label' => $this->label];
    }

    public function __unserialize(array $data): void {
        echo $data['label'], PHP_EOL;
        throw new Exception();
    }
}

class CatchingLegacy implements Serializable {
    public function __construct(private string $payload = '') {}

    public function serialize(): string {
        return $this->payload;
    }

    public function __serialize(): array {
        return ['payload' => $this->payload];
    }

    public function unserialize(string $data): void {
        try {
            unserialize($data);
        } catch (Exception) {
            echo "caught", PHP_EOL;
        }
    }

    public function __unserialize(array $data): void {
        $this->payload = $data['payload'];
    }
}

$payload = 'a:2:{i:0;' . custom_serialized(
    CatchingLegacy::class,
    serialize(new ThrowingPending('first')),
) . 'i:1;' . serialize(new ThrowingPending('second')) . '}';
var_dump(@unserialize($payload));
?>
--EXPECT--
nested
nested
outer
bool(true)
first
caught
bool(false)
