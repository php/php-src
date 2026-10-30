--TEST--
GH-18105 (Wrong line & file on error trace with trait default parameter)
--FILE--
<?php

file_put_contents(__DIR__ . '/gh18105_defs.inc', <<<'PHP'
<?php
class ThrowableParam {
    public function __construct() {
        throw new Exception('thrown here');
    }
}
trait TraitInASeparateFile {
    public function execute(ThrowableParam $param = new ThrowableParam): void {}
}
PHP);

include __DIR__ . '/gh18105_defs.inc';

class Foo {
    use TraitInASeparateFile;
}

try {
    (new Foo)->execute();
} catch (Exception $e) {
    echo basename($e->getFile()), "\n";
    echo $e->getLine(), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh18105_defs.inc');
?>
--EXPECT--
gh18105_defs.inc
4
