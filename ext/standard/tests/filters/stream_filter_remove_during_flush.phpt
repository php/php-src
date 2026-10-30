--TEST--
stream_filter_remove() of a filter that is removed while it is flushed
--FILE--
<?php
class Sink {
    public $context;

    public function stream_open($path, $mode, $options, &$opened_path): bool {
        return true;
    }

    public function stream_write(string $data): int {
        if (isset($GLOBALS['remove'])) {
            $filter = $GLOBALS['remove'];
            unset($GLOBALS['remove']);
            var_dump(stream_filter_remove($filter));
        }

        return strlen($data);
    }
}

stream_wrapper_register('sink', 'Sink');

$stream = fopen('sink://', 'w');
$filter = stream_filter_append($stream, 'convert.base64-encode', STREAM_FILTER_WRITE);
fwrite($stream, 'ab');
$GLOBALS['remove'] = $filter;
var_dump(stream_filter_remove($filter));
?>
--EXPECTF--
bool(true)

Warning: stream_filter_remove(): Filter has already been removed in %s on line %d
bool(false)
