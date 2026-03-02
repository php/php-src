--TEST--
SplFileObject::fread function
--FILE--
<?php
$obj = new SplFileObject(__FILE__, 'r');
$data = $obj->fread(5);
var_dump($data);

try {
    $data = $obj->fread(0);
    var_dump($data);
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

// read more data than is available
$data = $obj->fread(filesize(__FILE__) + 32);
var_dump(strlen($data) === filesize(__FILE__) - 5);

?>
--EXPECT--
string(5) "<?php"
SplFileObject::fread(): Argument #1 ($length) must be greater than 0
bool(true)
