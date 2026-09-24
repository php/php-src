--TEST--
IntlChar::enumCharNames() propagates callback exceptions and remains usable
--EXTENSIONS--
intl
--FILE--
<?php
try {
    IntlChar::enumCharNames(65, 68, static function ($codepoint) {
        echo $codepoint, "\n";
        throw new Exception('Stop enumeration');
    });
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
var_dump(IntlChar::enumCharNames(65, 68, static fn() => false));
?>
--EXPECT--
65
Stop enumeration
bool(true)
