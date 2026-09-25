--TEST--
GH-20733 (heap buffer overflow in optimizer)
--INI--
opcache.jit=tracing
opcache.jit_buffer_size=16M
--EXTENSIONS--
opcache
--FILE--
<?php
function test()
{
    $arr1 = ['line-break-chars' => 'php://temp','line-length' => $undef];
    $arr2 = [];
    $and = $arr1 and $arr2;

    while ($and) {
    }
}
echo "Done";
?>
--EXPECT--
Done
