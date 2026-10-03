--TEST--
GH-24088 (Class unions must not be inferred as numeric-only types)
--EXTENSIONS--
opcache
zend_test
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
error_reporting=E_ALL & ~E_NOTICE
--FILE--
<?php
function parameterUnion(int|NumericCastableNoOperations $value) {
    if ($value == 1) {
        $result = $value + 0;
        if (is_int($result)) {
            return $result;
        }
    }
}

function produceUnion(): int|NumericCastableNoOperations {
    return new NumericCastableNoOperations(6);
}

function returnUnion() {
    $value = produceUnion();
    if ($value == 1) {
        $result = $value + 0;
        if (is_int($result)) {
            return $result;
        }
    }
}

echo 'parameter union: ', parameterUnion(new NumericCastableNoOperations(6)), "\n";
echo 'return union: ', returnUnion(), "\n";
?>
--EXPECT--
parameter union: 6
return union: 6
