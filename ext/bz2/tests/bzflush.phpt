--TEST--
int bzflush ( resource $bz );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php
$bz = bzopen(dirname(__FILE__)."/003.txt.bz2","w");

if (bzflush($bz)) {
    print("OK");
} else {
    print("open bzip2 file has failed!");
}

bzclose($bz);
?>
--EXPECT--
OK
