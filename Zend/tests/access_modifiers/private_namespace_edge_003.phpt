--TEST--
Reflection: ReflectionProperty::isNamespacePrivateSet() for asymmetric visibility
--FILE--
<?php

namespace App {
    class Model {
        public private(namespace)(set) int $asymmetric = 1;
        private(namespace) int $regular = 2;
        public int $public = 3;
    }

    $rc = new \ReflectionClass(Model::class);

    $asym = $rc->getProperty('asymmetric');
    echo "asymmetric isPublic: " . ($asym->isPublic() ? 'true' : 'false') . "\n";
    echo "asymmetric isNamespacePrivateSet: " . ($asym->isNamespacePrivateSet() ? 'true' : 'false') . "\n";

    $reg = $rc->getProperty('regular');
    echo "regular isNamespacePrivate: " . ($reg->isNamespacePrivate() ? 'true' : 'false') . "\n";
    echo "regular isNamespacePrivateSet: " . ($reg->isNamespacePrivateSet() ? 'true' : 'false') . "\n";
}

?>
--EXPECT--
asymmetric isPublic: true
asymmetric isNamespacePrivateSet: true
regular isNamespacePrivate: true
regular isNamespacePrivateSet: false
