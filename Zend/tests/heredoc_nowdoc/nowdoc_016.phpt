--TEST--
Testing nowdocs with escape sequences
--FILE--
<?php

$test = <<<'TEST'
TEST;

var_dump(strlen($test));

$test = <<<'TEST'
\
TEST;

var_dump(strlen($test));

$test = <<<'TEST'
\0
TEST;

var_dump(strlen($test));

$test = <<<'TEST'
\n
TEST;

var_dump(strlen($test));

$test = <<<'TEST'
\\'
TEST;

var_dump(strlen($test));

?>
--EXPECT--
int(0)
int(1)
int(2)
int(2)
int(3)
