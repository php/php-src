--TEST--
Bug #68799 (Free called on uninitialized pointer)
--EXTENSIONS--
exif
--FILE--
<?php
/*
* Pollute the heap. Helps trigger bug. Sometimes not needed.
*/
class A {
    public $a;
    function __construct() {
        $a = 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa';
        $this->a = $a . $a . $a . $a . $a . $a;
    }
};

function doStuff ($limit) {

    $a = new A;

    $b = array();
    for ($i = 0; $i < $limit; $i++) {
        $b[$i] = clone $a;
    }

    unset($a);

    gc_collect_cycles();
}

$iterations = 3;

doStuff($iterations);
doStuff($iterations);

gc_collect_cycles();

print_r(exif_read_data(__DIR__.'/bug68799.jpg'));

?>
--EXPECTF--
Array
(
    [FileName] => bug68799.jpg
    [FileDateTime] => %d
    [FileSize] => 735
    [FileType] => 2
    [MimeType] => image/jpeg
    [SectionsFound] => ANY_TAG, IFD0, WINXP
    [COMPUTED] => Array
        (
            [html] => width="1" height="1"
            [Height] => 1
            [Width] => 1
            [IsColor] => 1
            [ByteOrderMotorola] => 1
        )

    [XResolution] => 96/1
    [YResolution] => 96/1
    [ResolutionUnit] => 2
    [Author] => 
)
