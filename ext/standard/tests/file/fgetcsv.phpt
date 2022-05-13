--TEST--
various fgetcsv() functionality tests
--FILE--
<?php
    $list = array(
        'aaa,bbb',
        'aaa,"bbb"',
        '"aaa","bbb"',
        'aaa,bbb',
        '"aaa",bbb',
        '"aaa",   "bbb"',
        ',',
        'aaa,',
        ',"aaa"',
        '"",""',
        '"\\"","aaa"',
        '"""""",',
        '""""",aaa' . "\n",
        '"\\""",aaa' . "\n",
        'aaa,"\\"bbb,ccc' . "\n",
        'aaa,bbb   ',
        'aaa,"bbb   "',
        'aaa"aaa","bbb"bbb',
        'aaa"aaa""",bbb',
        'aaa"\\"a","bbb"',
        'aaa,"bbb' . "\n",
        'aaa,"bbb',
    );

    $file = __DIR__ . '/fgetcsv.csv';
    @unlink($file);
    foreach ($list as $v) {
        $fp = fopen($file, "w");
        fwrite($fp, $v);
        fclose($fp);

        var_dump(fgetcsv(fopen($file, "r"), 1024));
    }
    @unlink($file);
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(3) "aaa"
}
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
array(2) {
  [0]=>
  string(2) "\""
  [1]=>
  string(3) "aaa"
}
array(2) {
  [0]=>
  string(2) """"
  [1]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(7) """,aaa
"
}
array(1) {
  [0]=>
  string(8) "\"",aaa
"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(10) "\"bbb,ccc
"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(6) "bbb   "
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(6) "bbb   "
}
array(2) {
  [0]=>
  string(8) "aaa"aaa""
  [1]=>
  string(6) "bbbbbb"
}
array(2) {
  [0]=>
  string(10) "aaa"aaa""""
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(8) "aaa"\"a""
  [1]=>
  string(3) "bbb"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(4) "bbb
"
}
array(2) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(3) "bbb"
}
