--TEST--
ReflectionZendExtension: basic test for __toString()
--SKIPIF--
<?php
$zendExtensions = get_loaded_extensions(true);
if (!in_array('Zend OPcache', $zendExtensions)) {
    die('SKIP the Zend OPcache extension not available');
}
?>
--FILE--
<?php
$rze = new ReflectionZendExtension('Zend OPcache');

$str = (string)$rze;

$methods = [
    'getName',
    'getVersion',
    'getAuthor',
    'getURL',
    'getCopyright',
];

foreach ($methods as $method) {
    $prop = $rze->{$method}();
    if (strpos($str, $prop) === false) {
        echo "The result of $method() ($prop) is not found in: $str\n";
    }
}
?>
===DONE===
--EXPECT--
===DONE===
