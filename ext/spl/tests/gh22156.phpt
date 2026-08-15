--TEST--
Bug GH-22156 SplFileObject::fgetcsv() with named escape parameter
--FILE--
<?php

echo "Test 1:\n";
$file = new SplFileObject('php://memory', 'rw+');

$file->fwrite("foo;bar;baz");
$file->seek(0);

$file->setCsvControl(';', "\n", "\\");

while (!$file->eof()) {
    $line = $file->fgetcsv(escape: '\\');
    var_dump($line);
}

echo "Test 2:\n";
$f = new SplFileObject('php://memory', 'rw+');
$f->setCsvControl(';', "\n", "\\");
$f->fwrite("a,b;c\n");
$f->seek(0);
var_dump($f->fgetcsv(',', '"', '\\'));

echo "Test 3:\n";
$f = new SplFileObject('php://memory', 'rw+');
$f->setCsvControl(',', "'", "\\");
$f->fwrite("a,'b,c'\n");
$f->seek(0);
var_dump($f->fgetcsv(escape: '\\'));

echo "Test 4:\n";
$f = new SplFileObject('php://memory', 'rw+');
$f->setCsvControl(',', "'");
$f->fwrite("a,'b\\'c'\n");
$f->seek(0);
var_dump($f->fgetcsv(escape: '\\'));

echo "Test 5:\n";  

$rm = new ReflectionMethod(SplFileObject::class, 'fgetcsv');
foreach ($rm->getParameters() as $p) {
    var_dump(
        $p->getName(),
        $p->allowsNull(),
        $p->getDefaultValue()
    );
}

echo "Test 6:\n";
$f = new SplFileObject('php://memory', 'rw+');
$f->setCsvControl(',', '"');
$f->fwrite("a,b\n");
$f->seek(0);
var_dump($f->fgetcsv());

?>
--EXPECTF--
Test 1:
array(3) {
  [0]=>
  string(3) "foo"
  [1]=>
  string(3) "bar"
  [2]=>
  string(3) "baz"
}
Test 2:
array(2) {
  [0]=>
  string(3) "a,b"
  [1]=>
  string(1) "c"
}
Test 3:
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(3) "b,c"
}
Test 4:

Deprecated: SplFileObject::setCsvControl(): the $escape parameter must be provided as its default value will change in %s on line %d
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(4) "b\'c"
}
Test 5:
string(9) "separator"
bool(false)
string(1) ","
string(9) "enclosure"
bool(false)
string(1) """
string(6) "escape"
bool(false)
string(1) "\"
Test 6:

Deprecated: SplFileObject::setCsvControl(): the $escape parameter must be provided as its default value will change in %s on line %d

Deprecated: SplFileObject::fgetcsv(): the $escape parameter must be provided, as its default value will change, either explicitly or via SplFileObject::setCsvControl() in %s on line %d
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
