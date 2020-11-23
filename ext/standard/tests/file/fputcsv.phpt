--TEST--
various fputcsv() functionality tests
--FILE--
<?php

$list = array (
  0 => 'aaa,bbb',
  1 => 'aaa,"bbb"',
  2 => '"aaa","bbb"',
  3 => 'aaa,bbb',
  4 => '"aaa",bbb',
  5 => '"aaa",   "bbb"',
  6 => ',',
  7 => 'aaa,',
  8 => ',"aaa"',
  9 => '"",""',
  10 => '"""""",',
  11 => '""""",aaa',
  12 => 'aaa,bbb   ',
  13 => 'aaa,"bbb   "',
  14 => 'aaa"aaa","bbb"bbb',
  15 => 'aaa"aaa""",bbb',
  16 => 'aaa,"\\"bbb,ccc',
  17 => 'aaa"\\"a","bbb"',
  18 => '"\\"","aaa"',
  19 => '"\\""",aaa',
);

$file = __DIR__ . '/fputcsv.csv';
@unlink($file);

$fp = fopen($file, "w");
foreach ($list as $v) {
    fputcsv($fp, explode(',', $v));
}
fclose($fp);

$res = file($file);
foreach($res as &$val)
{
    $val = substr($val, 0, -1);
}
echo '$list = ';var_export($res);echo ";\n";

$fp = fopen($file, "r");
$res = array();
while($l=fgetcsv($fp))
{
    $res[] = join(',',$l);
}
fclose($fp);

echo '$list = ';var_export($res);echo ";\n";

@unlink($file);

?>
--EXPECT--
$list = array (
  0 => 'aaa,bbb',
  1 => 'aaa,"""bbb"""',
  2 => '"""aaa""","""bbb"""',
  3 => 'aaa,bbb',
  4 => '"""aaa""",bbb',
  5 => '"""aaa""","   ""bbb"""',
  6 => ',',
  7 => 'aaa,',
  8 => ',"""aaa"""',
  9 => '"""""",""""""',
  10 => '"""""""""""""",',
  11 => '"""""""""""",aaa',
  12 => 'aaa,"bbb   "',
  13 => 'aaa,"""bbb   """',
  14 => '"aaa""aaa""","""bbb""bbb"',
  15 => '"aaa""aaa""""""",bbb',
  16 => 'aaa,"""\\"bbb",ccc',
  17 => '"aaa""\\"a""","""bbb"""',
  18 => '"""\\"""","""aaa"""',
  19 => '"""\\"""""",aaa',
);
$list = array (
  0 => 'aaa,bbb',
  1 => 'aaa,"bbb"',
  2 => '"aaa","bbb"',
  3 => 'aaa,bbb',
  4 => '"aaa",bbb',
  5 => '"aaa",   "bbb"',
  6 => ',',
  7 => 'aaa,',
  8 => ',"aaa"',
  9 => '"",""',
  10 => '"""""",',
  11 => '""""",aaa',
  12 => 'aaa,bbb   ',
  13 => 'aaa,"bbb   "',
  14 => 'aaa"aaa","bbb"bbb',
  15 => 'aaa"aaa""",bbb',
  16 => 'aaa,"\\"bbb,ccc',
  17 => 'aaa"\\"a","bbb"',
  18 => '"\\"","aaa"',
  19 => '"\\""",aaa',
);
