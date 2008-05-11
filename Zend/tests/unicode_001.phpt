--TEST--
Comparing size of binary and unicode strings
--FILE--
<?php 

$test = b<<<FOO
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = b<<<'FOO'
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = b<<<"FOO"
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = b"傀傂两亨乄了乆刄";

var_dump(strlen($test));

$test = (binary) "傀傂两亨乄了乆刄";

var_dump(strlen($test));

$test = <<<FOO
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = <<<'FOO'
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = <<<"FOO"
傀傂两亨乄了乆刄
FOO;

var_dump(strlen($test));

$test = "傀傂两亨乄了乆刄";

var_dump(strlen($test));

$test = "傀傂两亨乄了乆刄";

var_dump(strlen($test));

?>
--EXPECT--
int(24)
int(24)
int(24)
int(24)
int(24)
int(24)
int(24)
int(24)
int(24)
int(24)
--UEXPECT--
int(24)
int(24)
int(24)
int(24)
int(24)
int(8)
int(8)
int(8)
int(8)
int(8)
