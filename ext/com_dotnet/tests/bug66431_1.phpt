--TEST--
Bug #66431 Special Character via COM Interface (CP_UTF8), Application.Word
--EXTENSIONS--
com_dotnet
--SKIPIF--
<?php
try {
    new COM("word.application", NULL, CP_UTF8);
} catch (Exception $e) {
    die('skip ' . $e->getMessage());
}

?>
--FILE--
<?php

$text= "Xin chào cộng đồng PHP";
$fpath = str_replace("/", "\\", __DIR__ . "/bug66431.docx");

com_load_typelib('Word.Application');

$Wrd = new COM("word.application", NULL, CP_UTF8);
$Wrd->Documents->Add();
$Wrd->Selection->TypeText($text);
$Wrd->ActiveDocument->SaveAs($fpath);
$Wrd->ActiveDocument->Close(false);
$Wrd->Application->Quit();
unset($Wrd);

$Wrd = new COM("word.application", NULL, CP_UTF8);
$Wrd->Documents->Open($fpath, NULL, false);
$check_text = $Wrd->ActiveDocument->Range($Wrd->ActiveDocument->Sentences(1)->Start, $Wrd->ActiveDocument->Sentences(1)->End)->Text;
$Wrd->ActiveDocument->Close(false);
$Wrd->Application->Quit();
unset($Wrd);

/* trim the returned text as we'll get windows eol from a word doc. */
$result = (trim($check_text) == $text);

var_dump($result);

if (!$result) {
    echo "Expected: '$check_text'\n";
    echo "Have: '$text'\n";
}

?>
--CLEAN--
<?php

$fpath = str_replace("/", "\\", __DIR__ . "/bug66431.docx");

if (file_exists($fpath)) {
    unlink($fpath);
}
?>
--EXPECT--
bool(true)
