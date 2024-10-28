--TEST--
Test get_declared_enums() function
--FILE--
<?php

enum MyUnitEnum {}
enum MyStringBackedEnum: string {
    case FOO = 'test';
}
enum MyIntBackedEnum: int {
    case FOO = 1;
}
enum MyEnumImplementation implements Countable {
    use TraitWithNoProperties;
    public function count(): int {
        return 0;
    }
}

trait TraitWithNoProperties {}

$enumsList = get_declared_enums();
$classesList = get_declared_classes();
var_dump($enumsList);

foreach ($enumsList as $enum) {
    if (!enum_exists($enum)) {
        echo "Error: $enum is not a valid enum.\n";
    }
}

echo "Ensure all enums are included.\n";

var_dump(in_array('MyUnitEnum', $enumsList));
var_dump(in_array('MyStringBackedEnum', $enumsList));
var_dump(in_array('MyIntBackedEnum', $enumsList));
var_dump(in_array('MyEnumImplementation', $enumsList));

echo "Ensure interfaces are not included.\n";
var_dump(!in_array('Throwable', $enumsList));
var_dump(!in_array('IntBackedEnum', $enumsList));

echo "Ensure traits are not included.\n";
var_dump(!in_array('TraitWithNoProperties', $enumsList));

echo "Ensure classes are not included.\n";
var_dump(!in_array('stdClass', $enumsList));
var_dump(!in_array('Exception', $enumsList));

echo "Ensure enums are included in get_declared_classes().\n";
var_dump(in_array('MyUnitEnum', $classesList));
var_dump(in_array('MyStringBackedEnum', $classesList));
var_dump(in_array('MyIntBackedEnum', $classesList));
var_dump(in_array('MyEnumImplementation', $classesList));
echo "Done";
?>
--EXPECTF--
array(%d) {
%a
}
Ensure all enums are included.
bool(true)
bool(true)
bool(true)
bool(true)
Ensure interfaces are not included.
bool(true)
bool(true)
Ensure traits are not included.
bool(true)
Ensure classes are not included.
bool(true)
bool(true)
Ensure enums are included in get_declared_classes().
bool(true)
bool(true)
bool(true)
bool(true)
Done
