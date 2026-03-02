--TEST--
Bug #80933 (SplFileObject::DROP_NEW_LINE is broken for NUL and CR)
--FILE--
<?php
$lines = [
    "Lorem ipsum \0 dolor sit amet", // string with NUL
    "Lorem ipsum \r dolor sit amet", // string with CR
];
foreach ($lines as $line) {
    $temp = new SplTempFileObject();
    $temp->fwrite($line);

    $temp->rewind();
    $read = $temp->fgets();
    var_dump($line === $read);

    $temp->rewind();
    $temp->setFlags(SplFileObject::DROP_NEW_LINE);
    $read = $temp->fgets();
    var_dump($line === $read);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
