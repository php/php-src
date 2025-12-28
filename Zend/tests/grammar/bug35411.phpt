--TEST--
Bug #35411 (Regression with \{$ handling)
--FILE--
<?php
$abc = "bar";
echo "foo\{$abc}baz\n";
echo "foo\{ $abc}baz\n";
echo <<<TEST
foo{$abc}baz
foo\{$abc}baz
foo\{ $abc}baz
TEST;
?>
--EXPECT--
foo\{bar}baz
foo\{ bar}baz
foobarbaz
foo\{bar}baz
foo\{ bar}baz
