--TEST--
Generic trait: multiple type parameters and return type resolution
--FILE--
<?php
declare(strict_types=1);

trait Transform<In, Out> {
    public function transform(In $input): Out {
        return $this->doTransform($input);
    }

    abstract protected function doTransform(In $input): Out;
}

class StringToInt {
    use Transform<string, int>;

    protected function doTransform(string $input): int {
        return strlen($input);
    }
}

$t = new StringToInt();
echo $t->transform("hello") . "\n";

// Wrong input type
try {
    $t->transform(42);
} catch (TypeError $e) {
    echo "input: caught\n";
}

echo "OK\n";
?>
--EXPECT--
5
input: caught
OK
