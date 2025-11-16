--TEST--
bz2 filter assertion failure with non-dynamic properties in filter param object
--EXTENSIONS--
bz2
--FILE--
<?php

class ParamsCompress {
    public int $blocks = 5;
    public int $work = 10;
}

class ParamsDecompress {
    public bool $concatenated = true;
    public bool $small = true;
}

$fp = fopen('php://stdout', 'w');
stream_filter_append($fp, 'bzip2.compress', STREAM_FILTER_WRITE, new ParamsCompress);
stream_filter_append($fp, 'bzip2.decompress', STREAM_FILTER_WRITE, new ParamsDecompress);
fwrite($fp, "Hello world, hopefully not broken\n");

?>
--EXPECT--
Hello world, hopefully not broken
