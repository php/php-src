--TEST--
Bug #12414 ( extracting files from damaged archives)
--SKIPIF--
<?php
/*$ */
if(!extension_loaded('zip')) die('skip');
 ?>
--FILE--
<?php
$filename = 'MYLOGOV2.GFX';
$zipname = __DIR__ . "/pecl12414.zip";
$za = new ZipArchive();
$res =$za->open($zipname);
if ($res === TRUE) {
    $finfo=$za->statName($filename);
    $file_size=$finfo['size'];

    if($file_size>0) {
        $contents=$za->getFromName($filename);

        echo "ZIP contents size: " . strlen($contents) . "\n";
        if(strlen($contents)!=$file_size) {
            echo "zip_readfile recorded data does not match unpacked size: " . $zipname . " : " . $filename;
        }
    } else {
        $contents=false;
        echo "zip_readfile could not open stream from zero length file " . $zipname . " : " .$filename;
    }

    $za->close();
} else {
    echo "zip_readfile could not read from " . $zipname . " : " . $filename;
}

?>
--EXPECTF--
zip_readfile could not read from %specl12414.zip : MYLOGOV2.GFX
