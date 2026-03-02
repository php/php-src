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
    echo $e->getMessage(), "\n";
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
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Directly call fgetcsv ---
string(14) ""A", "B", "C"
"
string(13) ""D", "E", "F""
Cannot read from file php://temp
--- Use csv control ---
string(14) ""A", "B", "C"
"
string(13) ""D", "E", "F""
Cannot read from file php://temp
