--TEST--
SplTempFileObject iterates like SplFileObject at end of file
--FILE--
<?php
$path = __DIR__ . '/SplTempFileObject_iteration_eof.txt';
$files = [
    'SplFileObject' => new SplFileObject($path, 'w+'),
    'php://memory' => new SplFileObject('php://memory', 'w+'),
    'SplTempFileObject' => new SplTempFileObject(),
    'SplTempFileObject(0)' => new SplTempFileObject(0),
];

foreach ($files as $name => $file) {
    $file->fwrite("a\nb\nc\n");
    $file->rewind();

    $lines = [];
    foreach ($file as $key => $line) {
        $lines[] = $key . '=' . json_encode($line);
    }
    echo $name, ' foreach: ', implode(' ', $lines), "\n";

    $file->rewind();
    $lines = [];
    while (!$file->eof()) {
        $lines[] = json_encode($file->fgets());
    }
    echo $name, ' fgets: ', implode(' ', $lines), "\n";
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/SplTempFileObject_iteration_eof.txt');
?>
--EXPECT--
SplFileObject foreach: 0="a\n" 1="b\n" 2="c\n" 3=false
SplFileObject fgets: "a\n" "b\n" "c\n" ""
php://memory foreach: 0="a\n" 1="b\n" 2="c\n" 3=false
php://memory fgets: "a\n" "b\n" "c\n" ""
SplTempFileObject foreach: 0="a\n" 1="b\n" 2="c\n" 3=false
SplTempFileObject fgets: "a\n" "b\n" "c\n" ""
SplTempFileObject(0) foreach: 0="a\n" 1="b\n" 2="c\n" 3=false
SplTempFileObject(0) fgets: "a\n" "b\n" "c\n" ""
