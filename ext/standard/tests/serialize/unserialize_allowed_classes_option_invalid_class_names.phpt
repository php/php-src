--TEST--
Test unserialize() with array allowed_classes and nonsensical class names
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

try {
    unserialize($s, ["allowed_classes" => [""]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => ["245blerg"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => ["  whitespace  "]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => ["name\nwith whitespace"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => ['$dollars']]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    unserialize($s, ["allowed_classes" => ["have\0nul_byte"]]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: unserialize(): Option "allowed_classes" must be an array of class names, "  whitespace  " given
ValueError: unserialize(): Option "allowed_classes" must be an array of class names, "name
with whitespace" given
ValueError: unserialize(): Option "allowed_classes" must be an array of class names, "$dollars" given
ValueError: unserialize(): Option "allowed_classes" must be an array of class names, "have" given
