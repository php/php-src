--TEST--
Cloned UConverter resolves toUCallback/fromUCallback on the clone
--EXTENSIONS--
intl
--FILE--
<?php

class MyConverter extends UConverter {
    public int $hits = 0;

    public function toUCallback($reason, $source, $codeUnits, &$error): string|int|array|null {
        $this->hits++;
        return parent::toUCallback($reason, $source, $codeUnits, $error);
    }
}

$orig = new MyConverter('ascii', 'utf-8');
$clone = clone $orig;
$clone->convert("irregul\xC1\xA1r");
echo $clone->hits > 0 ? "ok\n" : "no callback\n";

?>
--EXPECT--
ok
