--TEST--
Reflection: ReflectionMethod::isNamespacePrivate() returns true for private(namespace) methods
--FILE--
<?php

namespace App {
    class Service {
        private(namespace) function test(): void {}
        private function priv(): void {}
        protected function prot(): void {}
        public function pub(): void {}
    }

    $rc = new \ReflectionClass(Service::class);

    $test = $rc->getMethod('test');
    echo "test isNamespacePrivate: " . ($test->isNamespacePrivate() ? 'true' : 'false') . "\n";
    echo "test isPrivate: " . ($test->isPrivate() ? 'true' : 'false') . "\n";

    $priv = $rc->getMethod('priv');
    echo "priv isNamespacePrivate: " . ($priv->isNamespacePrivate() ? 'true' : 'false') . "\n";
    echo "priv isPrivate: " . ($priv->isPrivate() ? 'true' : 'false') . "\n";
}

?>
--EXPECT--
test isNamespacePrivate: true
test isPrivate: false
priv isNamespacePrivate: false
priv isPrivate: true
