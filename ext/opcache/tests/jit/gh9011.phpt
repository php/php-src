--TEST--
GH-9011: Assertion failure with tracing JIT
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
$foo = [];
$foo[] = new \Exception(); /* Native interface implemented Native instance */
$foo[] = new class () implements \Stringable /* Native interface implemented User instance */
{
    public function __toString(): string
    {
        return "bar";
    }
};

foreach ($foo as $baz) {
    for ($i = 0; $i < 64; $i++) {
        $baz->__toString();
    }
}
?>
DONE
--EXPECT--
DONE