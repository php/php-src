--TEST--
imageaffinematrixget() array|float parameter coercions (strict mode)
--EXTENSIONS--
gd
--FILE--
<?php

declare(strict_types=1);

$values = [
    'null' => null,
    'false' => false,
    'true' => true,
    'int' => 42,
    'float' => 73.5,
    'numeric string' => '15',
    'non-numeric string' => 'string',
    'array' => [],
    'object' => new stdClass(),
];

foreach ($values as $name => $value) {
    echo "$name:\n";
    try {
        imageaffinematrixget(IMG_AFFINE_ROTATE, $value);
        echo "accepted\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

echo "array for translate:\n";
imageaffinematrixget(IMG_AFFINE_TRANSLATE, ['x' => 1, 'y' => 2]);
echo "accepted\n";

echo "float for translate:\n";
try {
    imageaffinematrixget(IMG_AFFINE_TRANSLATE, 1.0);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
null:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, null given
false:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, false given
true:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, true given
int:
accepted
float:
accepted
numeric string:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, string given
non-numeric string:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, string given
array:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type float when using rotate or shear
object:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, stdClass given
array for translate:
accepted
float for translate:
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array when using translate or scale
