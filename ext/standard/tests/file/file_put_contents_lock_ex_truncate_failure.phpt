--TEST--
file_put_contents() LOCK_EX stops writing after a wrapper truncation failure
--FILE--
<?php
class FailingTruncateWrapper
{
    public $context;
    public static bool $truncateResult = false;

    public function stream_open($path, $mode, $options, &$openedPath)
    {
        return true;
    }

    public function stream_lock($operation)
    {
        return true;
    }

    public function stream_truncate($newSize)
    {
        echo "truncate: $newSize\n";
        return self::$truncateResult;
    }

    public function stream_write($data)
    {
        echo "write\n";
        return strlen($data);
    }

    public function stream_close()
    {
        echo "close\n";
    }
}

$source = fopen('php://memory', 'w+');
fwrite($source, 'data');
rewind($source);

var_dump(stream_wrapper_unregister('file'));
var_dump(stream_wrapper_register('file', FailingTruncateWrapper::class));
foreach (['string' => 'data', 'empty' => '', 'array' => ['data'], 'stream' => $source] as $label => $data) {
    echo "$label:\n";
    var_dump(file_put_contents('file:///test', $data, LOCK_EX));
}
echo "source position: ", ftell($source), "\n";
fclose($source);

echo "successful truncation:\n";
FailingTruncateWrapper::$truncateResult = true;
var_dump(file_put_contents('file:///test', 'data', LOCK_EX));

echo "append:\n";
FailingTruncateWrapper::$truncateResult = false;
var_dump(file_put_contents('file:///test', 'data', FILE_APPEND | LOCK_EX));
stream_wrapper_restore('file');
?>
--EXPECT--
bool(true)
bool(true)
string:
truncate: 0
close
bool(false)
empty:
truncate: 0
close
bool(false)
array:
truncate: 0
close
bool(false)
stream:
truncate: 0
close
bool(false)
source position: 0
successful truncation:
truncate: 0
write
close
int(4)
append:
write
close
int(4)
