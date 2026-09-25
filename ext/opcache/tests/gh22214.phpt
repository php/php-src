--TEST--
GH-22214: opcache.dups_fix is honored for duplicate functions
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.dups_fix=1
--FILE--
<?php
file_put_contents(__DIR__ . "/gh22214.inc", <<<'PHP'
<?php
function test_gh22214() {
    echo "test_gh22214 called\n";
}
class TestGh22214 {
    public function hello() {
        echo "TestGh22214 called\n";
    }
}
PHP
);

include __DIR__ . "/gh22214.inc";
include __DIR__ . "/gh22214.inc";

test_gh22214();
(new TestGh22214())->hello();
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/gh22214.inc");
?>
--EXPECT--
test_gh22214 called
TestGh22214 called
