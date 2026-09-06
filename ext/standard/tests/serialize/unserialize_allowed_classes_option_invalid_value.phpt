--TEST--
Test unserialize() with non-bool/array allowed_classes
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

try {
    unserialize($s, ["allowed_classes" => null]);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

try {
    unserialize($s, ["allowed_classes" => 0]);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}


try {
    unserialize($s, ["allowed_classes" => 1]);
} catch (Throwable $exception) {
    echo $exception::class, ': ', $exception->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: unserialize(): Option "allowed_classes" must be of type array|bool, null given
TypeError: unserialize(): Option "allowed_classes" must be of type array|bool, int given
TypeError: unserialize(): Option "allowed_classes" must be of type array|bool, int given
