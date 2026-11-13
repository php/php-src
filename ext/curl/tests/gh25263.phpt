--TEST--
GH-25263 (Use-after-free of __call trampoline when cloning CurlHandle)
--EXTENSIONS--
curl
--FILE--
<?php
class T {
    public function __call($name, $args) {
        echo $name, PHP_EOL;
        return strlen($args[1]);
    }
}

$ch = curl_init('file://' . __FILE__);
curl_setopt($ch, CURLOPT_WRITEFUNCTION, [new T, 'trampoline']);
$ch2 = clone $ch;
unset($ch);
curl_exec($ch2);

$ch = curl_init();
curl_setopt($ch, CURLOPT_READFUNCTION, [new T, 'trampoline']);
$ch2 = clone $ch;
unset($ch2);
echo "Done\n";
?>
--EXPECT--
trampoline
Done
