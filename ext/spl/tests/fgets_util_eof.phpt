--TEST--
SplFileObject::fgets() util eof
--FILE--
<?php
$file = new SplFileObject(tempnam(sys_get_temp_dir(), 'empty_'), 'r+');
try {
    var_dump($file->fgets());
    echo "Never here\n";
} catch (RuntimeException) { }
var_dump($file->eof());

$file = new SplFileObject(__FILE__);
while (true) {
    try {
        $file->fgets();
    } catch (RuntimeException) {
        break;
    }
}
var_dump($file->eof());

echo "OK\n";

?>
--EXPECT--
bool(true)
bool(true)
OK
