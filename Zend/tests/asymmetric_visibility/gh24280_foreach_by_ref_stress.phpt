--TEST--
GH-24280: repeatedly deny scoped property references while preserving legal partial writes
--FILE--
<?php
class ReferenceStressObject {
    public string $visible = 'before';
    public private(set) int $restricted = 7;
    private static array $retained = [];

    public function retain(): void {
        self::$retained[] =& $this->restricted;
    }

    public function updateInternally(): void {
        foreach ($this as &$value) {
            if (is_int($value)) {
                $value = 8;
            }
        }
        unset($value);
    }
}

function attemptExternalMutation(ReferenceStressObject $object): void {
    try {
        foreach ($object as &$value) {
            if (is_string($value)) {
                $value = 'after';
            } else {
                $value = 99;
            }
        }
        unset($value);
        throw new LogicException('Forbidden reference accepted');
    } catch (Error $error) {
        if (!str_contains($error->getMessage(), 'Cannot indirectly modify private(set) property ReferenceStressObject::$restricted')) {
            throw $error;
        }
        unset($value);
    }
    if ($object->restricted !== 7 || $object->visible !== 'after') {
        throw new LogicException('Incorrect property state after denied reference');
    }
}

for ($i = 0; $i < 3000; $i++) {
    $object = new ReferenceStressObject();
    if (($i & 1) === 0) {
        $object->retain();
    }
    attemptExternalMutation($object);
    $object->updateInternally();
    if ($object->restricted !== 8) {
        throw new LogicException('Legal in-class reference was rejected');
    }
}
echo "3000 scoped reference checks and legal writes: PASS\n";
?>
--EXPECT--
3000 scoped reference checks and legal writes: PASS
