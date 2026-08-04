--TEST--
GH-21742 (SplFileObject::fgets() throws at EOF in while (!$spl->eof()) loop)
--FILE--
<?php
$file = tempnam(sys_get_temp_dir(), 'spl');
file_put_contents($file, "Line 0\nLine 1\nLine 2\nLine 3\nLine 4\n");

$spl = new SplFileObject($file, 'r');
while (!$spl->eof()) {
    echo $spl->fgets();
}
echo "clean exit\n";

$empty = tempnam(sys_get_temp_dir(), 'spl');
file_put_contents($empty, '');
$spl2 = new SplFileObject($empty, 'r');
$iter = 0;
while (!$spl2->eof()) {
    $iter++;
    $spl2->fgets();
    if ($iter > 3) break;
}
echo "empty-file iters=$iter\n";

unlink($file);
unlink($empty);
?>
--EXPECT--
Line 0
Line 1
Line 2
Line 3
Line 4
clean exit
empty-file iters=1
