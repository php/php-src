--TEST--
fputcsv() variant where escape parameter matters
--FILE--
<?php

$list = [
  1 => 'aaa,"/"bbb,ccc',
  2 => 'aaa"/"a","bbb"',
  3 => '"/"","aaa"',
  4 => '"/""",aaa',
];

$file = __DIR__ . '/fputcsv_variation18.csv';

$fp = fopen($file, "w");
foreach ($list as $v) {
    fputcsv($fp, explode(',', $v), ',', '"', '/');
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
while($l=fgetcsv($fp, 0, ',', '"', '/'))
{
    $res[] = join(',',$l);
}
fclose($fp);

echo '$list = ';var_export($res);echo ";\n";

?>
--CLEAN--
<?php
$file = __DIR__ . '/fputcsv_variation18.csv';
@unlink($file);
?>
--EXPECTF--
$list = array (
  0 => 'aaa,"""/"bbb",ccc',
  1 => '"aaa""/"a""","""bbb"""',
  2 => '"""/"""","""aaa"""',
  3 => '"""/"""""",aaa',
);
$list = array (
  0 => 'aaa,"/"bbb,ccc',
  1 => 'aaa"/"a","bbb"',
  2 => '"/"","aaa"',
  3 => '"/""",aaa',
);
