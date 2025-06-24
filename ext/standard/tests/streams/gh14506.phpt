--TEST--
GH-14506 (Closing a userspace stream inside a userspace handler causes heap corruption)
--FILE--
<?php

class Bomb {

    public $context;

    function stream_open($path, $mode, $options, &$opened_path): bool
    {
        return true;
    }

    function stream_write(string $data): int
    {
        global $readStream;
        fclose($readStream);
        return 0;
    }

    function stream_read(int $count): false|string|null
    {
        global $readStream;
        fclose($readStream);
        return "";
    }

    function stream_eof(): bool
    {
        global $readStream;
        fclose($readStream);
        return false;
    }

    function stream_seek(int $offset, int $whence): bool
    {
        global $readStream;
        fclose($readStream);
        return false;
    }
    
    function stream_cast(int $as)
    {
        global $readStream;
        fclose($readStream);
        return false;
    }

    function stream_flush(): bool
    {
        global $readStream;
        fclose($readStream);
        return false;
    }
}

stream_register_wrapper('bomb', Bomb::class);
$readStream = fopen('bomb://1', 'r');
fread($readStream, 1);
fwrite($readStream, "x", 1);
fseek($readStream, 0, SEEK_SET);
$streams = [$readStream];
$empty = [];
try {
    stream_select($streams, $streams,$empty, 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
fflush($readStream);
try {
    fclose($readStream);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s on line %d

Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s on line %d
No stream arrays were passed
fclose(): Argument #1 ($stream) must be an open stream resource
