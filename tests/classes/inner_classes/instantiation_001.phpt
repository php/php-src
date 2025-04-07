--TEST--
instantiation from various scopes
--FILE--
<?php

class Outer {
    private class PrivateInner {
    }
    protected class ProtectedInner {
    }
    public class PublicInner {
    }
}

try {
    var_dump(new Outer\PrivateInner());
} catch (Throwable $e) {
    echo "Failed to instantiate Outer\PrivateInner: {$e->getMessage()}\n";
}

try {
    var_dump(new Outer\ProtectedInner());
} catch (Throwable $e) {
    echo "Failed to instantiate Outer\ProtectedInner: {$e->getMessage()}\n";
}

try {
    var_dump(new Outer\PublicInner());
} catch (Throwable $e) {
    echo "Failed to instantiate Outer\PublicInner: {$e->getMessage()}\n";
}

class Other {
    public function testPrivate() {
        var_dump(new Outer\PrivateInner());
    }
    public function testProtected() {
        var_dump(new Outer\ProtectedInner());
    }
    public function testPublic() {
        var_dump(new Outer\PublicInner());
    }
}

$other = new Other();
foreach (['Private', 'Protected', 'Public'] as $type) {
    try {
        $other->{"test$type"}();
    } catch(Throwable $e) {
        echo "Failed to instantiate Outer\\$type: {$e->getMessage()}\n";
    }
}

class Child extends Outer {
    public function testPrivate() {
        var_dump(new Outer\PrivateInner());
    }
    public function testProtected() {
        var_dump(new Outer\ProtectedInner());
    }
    public function testPublic() {
        var_dump(new Outer\PublicInner());
    }
}

$other = new Child();
foreach (['Private', 'Protected', 'Public'] as $type) {
    try {
        $other->{"test$type"}();
    } catch(Throwable $e) {
        echo "Failed to instantiate Outer\\$type: {$e->getMessage()}\n";
    }
}

?>
--EXPECT--
Failed to instantiate Outer\PrivateInner: Cannot instantiate class Outer\PrivateInner from the global scope
Failed to instantiate Outer\ProtectedInner: Cannot instantiate class Outer\ProtectedInner from the global scope
object(Outer\PublicInner)#1 (0) {
}
Failed to instantiate Outer\Private: Cannot instantiate private class Outer\PrivateInner from scope Other
Failed to instantiate Outer\Protected: Cannot instantiate protected class Outer\ProtectedInner from scope Other
object(Outer\PublicInner)#3 (0) {
}
Failed to instantiate Outer\Private: Cannot instantiate private class Outer\PrivateInner from scope Child
object(Outer\ProtectedInner)#2 (0) {
}
object(Outer\PublicInner)#2 (0) {
}
