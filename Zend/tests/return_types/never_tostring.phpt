--TEST--
never type of __toString method
--FILE--
<?php

class A implements Stringable {
    public function __toString(): string {
        return "hello";
    }
}

class B extends A {
    public function __toString(): never {
        throw new \Exception('not supported');
    }
}

try {
    echo (string) (new B());
} catch (Exception $e) {
    // do nothing
}

echo "done";

?>
--EXPECT--
done
