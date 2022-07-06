--TEST--
SPL: RecursiveDirectoryIterator with CURRENT_AS_PATHNAME flag
--CREDITS--
Paul Garvin pgarvin76@gmail.com
--FILE--
<?php
$td = __DIR__ . '/bug66405';
mkdir($td);
touch($td . '/file1.txt');
touch($td . '/file2.md');
mkdir($td . '/testsubdir');
touch($td . '/testsubdir/file3.csv');

class Bug66405 extends RecursiveDirectoryIterator
{
    public function current()
    {
        $current = parent::current();
        echo gettype($current) . " $current\n";
        return $current;
    }

    public function getChildren()
    {
        $children = parent::getChildren();
        if (is_object($children)) {
            echo get_class($children) . " $children\n";
        } else {
            echo gettype($children) . " $children\n";
        }
        return $children;
    }
}

$rdi = new Bug66405($td, FilesystemIterator::CURRENT_AS_PATHNAME | FilesystemIterator::SKIP_DOTS);
$rii = new RecursiveIteratorIterator($rdi);

ob_start();
foreach ($rii as $file) {
    //noop
}
$results = explode("\n", ob_get_clean());
sort($results);
echo implode("\n", $results);
?>
--CLEAN--
<?php
$td = __DIR__ . '/bug66405';
unlink($td . '/testsubdir/file3.csv');
unlink($td . '/file2.md');
unlink($td . '/file1.txt');
rmdir($td . '/testsubdir');
rmdir($td);
?>
--EXPECTF--
Bug66405 file3.csv
string %sbug66405%efile1.txt
string %sbug66405%efile2.md
string %sbug66405%etestsubdir%efile3.csv
