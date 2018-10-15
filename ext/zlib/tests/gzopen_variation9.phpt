--TEST--
Test gzopen() function : variation: try opening with possibly invalid modes
--SKIPIF--
<?php
if (!extension_loaded("zlib")) {
	print "skip - ZLIB extension not loaded";
}
?>
--FILE--
<?php
/* Prototype  : resource gzopen(string filename, string mode [, int use_include_path])
 * Description: Open a .gz-file and return a .gz-file pointer
 * Source code: ext/zlib/zlib.c
 * Alias to functions:
 */

echo "*** Testing gzopen() : variation ***\n";

$modes = array('r+', 'rf', 'w+' , 'e');

$file = dirname(__FILE__)."/004.txt.gz";

foreach ($modes as $mode) {
    echo "mode=$mode\n";
    $h = gzopen($file, $mode);
    echo "gzopen=";
    var_dump($h);
    if ($h !== false) {
       gzclose($h);
    }
    echo "\n";
}
?>
===DONE===
--EXPECTF--
*** Testing gzopen() : variation ***
mode=r+

Warning: gzopen(): cannot open a zlib stream for reading and writing at the same time! in %s on line %d
gzopen=bool(false)

mode=rf
gzopen=resource(%d) of type (stream)

mode=w+

Warning: gzopen(): cannot open a zlib stream for reading and writing at the same time! in %s on line %d
gzopen=bool(false)

mode=e

Warning: gzopen(%s/004.txt.gz): failed to open stream: %s in %s on line %d
gzopen=bool(false)

===DONE===
