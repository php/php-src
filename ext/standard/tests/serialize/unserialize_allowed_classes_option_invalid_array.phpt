--TEST--
Test unserialize() with array allowed_classes and nonsensical values
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

try {
    unserialize($s, ["allowed_classes" => [null]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [false]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [true]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [42]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [15.2]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [[]]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [STDERR]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => [new stdClass]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, null given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, false given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, true given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, int given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, float given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, array given
TypeError: unserialize(): Option "allowed_classes" must be an array of class names, resource given
Error: Object of class stdClass could not be converted to string
