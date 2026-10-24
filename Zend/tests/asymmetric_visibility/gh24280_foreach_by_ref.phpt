--TEST--
GH-24280: foreach by reference must respect asymmetric setter visibility
--FILE--
<?php
class PrivateSetter {
    public private(set) string $value = 'original';

    public function updateInside(): void {
        foreach ($this as &$value) {
            $value = 'inside';
        }
        unset($value);
    }

    private static array $retainedReferences = [];

    public function prepareReference(): void {
        self::$retainedReferences[] =& $this->value;
    }
}

class PrivateSetterChild extends PrivateSetter {
    public function updateFromChild(): void {
        foreach ($this as &$value) {
            $value = 'child';
        }
        unset($value);
    }
}

class ProtectedSetter {
    public protected(set) int $value = 10;
}

class ProtectedSetterChild extends ProtectedSetter {
    public function updateInside(): void {
        foreach ($this as &$value) {
            ++$value;
        }
        unset($value);
    }
}

class PublicSetter {
    public string $value = 'initial';
}

class ReadonlyProperty {
    public readonly int $value;

    public function __construct() {
        $this->value = 1;
    }
}

function acquireReference(object $object): void {
    try {
        foreach ($object as &$value) {
            if (is_string($value)) {
                $value = 'outside';
            } else {
                ++$value;
            }
        }
        unset($value);
        echo "accepted\n";
    } catch (Error $error) {
        echo $error->getMessage(), "\n";
    }
}

$private = new PrivateSetter();
echo "private outside: ";
acquireReference($private);
var_dump($private->value);
$private->updateInside();
var_dump($private->value);

$existingReference = new PrivateSetter();
$existingReference->prepareReference();
if (ReflectionReference::fromArrayElement((array) $existingReference, 'value') === null) {
    throw new LogicException('Test setup failed: value is not a persistent reference');
}
echo "existing reference outside: ";
acquireReference($existingReference);
var_dump($existingReference->value);
$existingReference->updateInside();
var_dump($existingReference->value);

$privateChild = new PrivateSetterChild();
echo "private child: ";
try {
    $privateChild->updateFromChild();
    echo "accepted\n";
} catch (Error $error) {
    echo $error->getMessage(), "\n";
}
var_dump($privateChild->value);

$protected = new ProtectedSetterChild();
echo "protected outside: ";
acquireReference($protected);
var_dump($protected->value);
$protected->updateInside();
var_dump($protected->value);

$public = new PublicSetter();
echo "public outside: ";
acquireReference($public);
var_dump($public->value);

$readonly = new ReadonlyProperty();
echo "readonly outside: ";
acquireReference($readonly);
var_dump($readonly->value);
?>
--EXPECT--
private outside: Cannot indirectly modify private(set) property PrivateSetter::$value from global scope
string(8) "original"
string(6) "inside"
existing reference outside: Cannot indirectly modify private(set) property PrivateSetter::$value from global scope
string(8) "original"
string(6) "inside"
private child: Cannot indirectly modify private(set) property PrivateSetter::$value from scope PrivateSetterChild
string(8) "original"
protected outside: Cannot indirectly modify protected(set) property ProtectedSetter::$value from global scope
int(10)
int(11)
public outside: accepted
string(7) "outside"
readonly outside: Cannot acquire reference to readonly property ReadonlyProperty::$value
int(1)
