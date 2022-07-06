--TEST--
Bug #72014 (Including a file with anonymous classes multiple times leads to fatal error)
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
file_put_contents(__DIR__ . "/bug72014.annon.php", <<<PHP
<?php
\$a = new class() { public \$testvar = "Foo\n"; };
echo \$a->testvar;
PHP
);

include(__DIR__ . "/bug72014.annon.php");
include(__DIR__ . "/bug72014.annon.php");
include(__DIR__ . "/bug72014.annon.php");
?>
--CLEAN--
<?php
@unlink(__DIR__ . "/bug72014.annon.php")
?>
--EXPECT--
Foo
Foo
Foo
