--TEST--
Dynamically defined interfaces only affect classes defined later
--FILE--
<?php

class AncientClass implements ?OptionalInterface {}
$ancientClass = new AncientClass;
echo 'AC implements:'.implode(', ', class_implements('AncientClass')),"\n";
echo 'AC object implements:'.implode(', ', class_implements($ancientClass)),"\n\n";

eval('interface OptionalInterface {}');
echo "Interface defined\n\n";

class NewClass implements ?OptionalInterface {}

// Newly defined class implements the interface
$newClass = new NewClass;
echo 'NC implements:'.implode(', ', class_implements('NewClass')),"\n";
echo 'NC implements:'.implode(', ', class_implements($newClass)),"\n\n";

// The old class doesn't
echo 'AC object implements:'.implode(', ', class_implements($ancientClass)),"\n";

echo 'AC implements:'.implode(', ', class_implements('AncientClass')),"\n";

$ancientClass2 = new AncientClass;
echo 'New AC object implements:'.implode(', ', class_implements($ancientClass2)),"\n";

?>
--EXPECT--
AC implements:
AC object implements:

Interface defined

NC implements:OptionalInterface
NC implements:OptionalInterface

AC object implements:
AC implements:
New AC object implements:
