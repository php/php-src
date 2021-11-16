--TEST--
Bug #66322 (COMPersistHelper::SaveToFile can save to wrong location)
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
try {
    new COM('Word.Application');
} catch (com_exception $ex) {
    die('skip MS Word not available');
}
if (getenv("SKIP_SLOW_TESTS")) die('skip slow test');
?>
--FILE--
<?php
$w = new COM('Word.Application');
$doc = $w->Documents->Add();
$ph = new COMPersistHelper($doc);
$filename = __DIR__ . '\\..\\' . basename(__DIR__) . '\\66322.docx';
$ph->SaveToFile($filename);
var_dump(file_exists($filename));
$w->Quit();
?>
--EXPECT--
bool(true)
