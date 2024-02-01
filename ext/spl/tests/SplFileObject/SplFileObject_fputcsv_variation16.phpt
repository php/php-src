--TEST--
SplFileObject::fputcsv() with user provided eol
--FILE--
<?php
$data = [
    ['aaa', 'bbb', 'ccc', 'dddd'],
    ['123', '456', '789'],
    ['"aaa"', '"bbb"'],
];

$eol_chars = ['||', '|', '\n', "\n"];
foreach ($eol_chars as $eol_char) {
  $file = new SplTempFileObject;
    foreach ($data as $record) {
        $file->fputcsv($record, ',', '"', '', $eol_char);
    }

    $file->rewind();
    foreach ($file as $line) {
        echo $line;
    }
    
    echo "\n";
}
?>
--EXPECT--
aaa,bbb,ccc,dddd||123,456,789||"""aaa""","""bbb"""||
aaa,bbb,ccc,dddd|123,456,789|"""aaa""","""bbb"""|
aaa,bbb,ccc,dddd\n123,456,789\n"""aaa""","""bbb"""\n
aaa,bbb,ccc,dddd
123,456,789
"""aaa""","""bbb"""
