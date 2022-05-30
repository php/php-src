--TEST--
GH-8121 (SplFileObject - seek and key with csv file inconsistent)
--FILE--
<?php
$flagss = [
    SplFileObject::READ_AHEAD | SplFileObject::READ_CSV | SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE,
    SplFileObject::READ_AHEAD |                           SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE,
                                                          SplFileObject::SKIP_EMPTY | SplFileObject::DROP_NEW_LINE,
];
foreach ($flagss as $flags) {
    $file = new SplFileObject(__DIR__ . "/gh8121.csv", "r");
    echo "flags: $flags\n";
    $file->setFlags($flags);
    $file->seek(0);
    var_dump($file->key());
    $file->seek(1);
    var_dump($file->key());
    $file->seek(2);
    var_dump($file->key());
    $file->seek(3);
    var_dump($file->key());
}
?>
--EXPECT--
flags: 15
int(0)
int(1)
int(2)
int(3)
flags: 7
int(0)
int(1)
int(2)
int(3)
flags: 5
int(0)
int(1)
int(2)
int(3)
