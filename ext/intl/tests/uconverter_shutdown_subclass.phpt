--TEST--
UConverter subclass destroyed at request shutdown does not crash
--EXTENSIONS--
intl
--FILE--
<?php
class MyConverter extends UConverter {
    public function toUCallback($reason, $source, $codeUnits, &$error): string|int|array|null {
        return '?';
    }

    public function fromUCallback($reason, $source, $codePoint, &$error): string|int|array|null {
        return '?';
    }
}

$converter = new MyConverter('ascii', 'utf-8');
echo 'end of script', PHP_EOL;
?>
--EXPECT--
end of script
