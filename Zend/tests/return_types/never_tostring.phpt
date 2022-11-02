--TEST--
never type of __toBool method
--FILE--
<?php

class A implements Falsifiable {
    public function __toBool(): bool {
        return true;
    }
}

class B extends A {
    public function __toBool(): never {
        throw new \Exception('not supported');
    }
}

try {
    echo (bool) (new B());
} catch (Exception $e) {
    // do nothing
}

echo "done";

?>
--EXPECT--
done
