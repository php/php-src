--TEST--
Reflection: ReflectionProperty::isNamespacePrivate() returns true for private(namespace) properties
--FILE--
<?php

namespace App {
    class Model {
        private(namespace) int $nsPriv = 1;
        private int $priv = 2;
        protected int $prot = 3;
        public int $pub = 4;
    }

    $rc = new \ReflectionClass(Model::class);

    $nsPriv = $rc->getProperty('nsPriv');
    echo "nsPriv isNamespacePrivate: " . ($nsPriv->isNamespacePrivate() ? 'true' : 'false') . "\n";
    echo "nsPriv isPrivate: " . ($nsPriv->isPrivate() ? 'true' : 'false') . "\n";

    $priv = $rc->getProperty('priv');
    echo "priv isNamespacePrivate: " . ($priv->isNamespacePrivate() ? 'true' : 'false') . "\n";
    echo "priv isPrivate: " . ($priv->isPrivate() ? 'true' : 'false') . "\n";
}

?>
--EXPECT--
nsPriv isNamespacePrivate: true
nsPriv isPrivate: false
priv isNamespacePrivate: false
priv isPrivate: true
