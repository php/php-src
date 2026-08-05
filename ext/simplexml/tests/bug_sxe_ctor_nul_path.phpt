--TEST--
SimpleXMLElement constructor rejects embedded NUL in URL/path mode
--EXTENSIONS--
simplexml
--FILE--
<?php
$tmp = tempnam(sys_get_temp_dir(), 'sxe');
file_put_contents($tmp, '<r/>');
$path = $tmp . "\0evil";
try {
    new SimpleXMLElement($path, 0, true);
    echo "ctor: loaded\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
try {
    simplexml_load_file($path);
    echo "load_file: loaded\n";
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), "\n";
}
unlink($tmp);
?>
--EXPECT--
ValueError: SimpleXMLElement::__construct(): Argument #1 ($data) must not contain any null bytes
ValueError: simplexml_load_file(): Argument #1 ($filename) must not contain any null bytes
