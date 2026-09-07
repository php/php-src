--TEST--
GH-13685 (Unexpected null pointer in zend_string.h)
--FILE--
<?php

$contents = <<<EOS
"A", "B", "C"
"D", "E", "F"
EOS;

echo "--- Directly call fgetcsv ---\n";

$file = new SplTempFileObject;
$file->fwrite($contents);
$file->rewind();
while (($data = $file->fgetcsv(',', '"', ''))) {
    var_dump((string) $file);
}
try {
    var_dump((string) $file);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- Use csv control ---\n";

$file = new SplTempFileObject;
$file->fwrite($contents);
$file->rewind();
$file->setFlags(SplFileObject::READ_CSV);
$file->setCsvControl(',', '"', '');
foreach ($file as $row) {
    var_dump((string) $file);
}
try {
    var_dump((string) $file);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
--- Directly call fgetcsv ---

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
string(14) ""A", "B", "C"
"

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
string(13) ""D", "E", "F""

Deprecated: Method SplFileObject::fgetcsv() is deprecated since 8.6 in %s on line %d
string(0) ""
--- Use csv control ---

Deprecated: Method SplFileObject::setCsvControl() is deprecated since 8.6 in %s on line %d
string(14) ""A", "B", "C"
"
string(13) ""D", "E", "F""
string(0) ""
