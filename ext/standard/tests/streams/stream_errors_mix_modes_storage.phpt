--TEST--
Stream errors - silent mode with mixed error stores
--FILE--
<?php

class TestStream {
    public $context;
    private $position = 0;

    public function stream_open($path, $mode, $options, &$opened_path) {
        return true;
    }

    public function stream_read($count) {
        $this->position += $count;
        return str_repeat('x', $count + 10);
    }

    public function stream_eof() {
        return $this->position >= 100;
    }

    public function stream_stat() {
        return [];
    }
}

stream_wrapper_register('test', 'TestStream');

function stream_test_errors($title, $contextOptions) {
    $context = stream_context_create($contextOptions);
    $stream = fopen('test://foo', 'r', false, $context);
    try {
        echo $title . "\n";
        $readin = fopen('php://stdin', 'r', false, $context);
        $data = fread($stream, 10);

        $read = [$readin, $stream];
        $write = NULL;
        $except = NULL;
        stream_select($read, $write, $except, 0, 0, $context);
    } catch (StreamException $e) {
        echo 'EXCEPTION: ' . $e->getMessage() . "\n";
    }

    $error = stream_get_last_error();
    if ($error) {
        echo "Error details:\n";
        echo "- Message: " . $error->message . "\n";
        echo "- Code: $error->code\n";
        echo "- Wrapper: " . $error->wrapperName . "\n";
        echo "- Terminating: " . ($error->terminating ? 'yes' : 'no') . "\n";
        echo "- Count: " . $error->count() . "\n";
        
        // Show all errors in chain
        $current = $error;
        $idx = 0;
        while ($current) {
            echo "  [$idx] " . $current->code . ": " . $current->message . "\n";
            $current = $current->next;
            $idx++;
        }
    } else {
        echo "No errors stored\n";
    }
    echo "\n";
}

stream_test_errors('ALL', [
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::All,
    ]
]);

stream_test_errors('NON TERMINATING', [
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::NonTerminating,
    ]
]);

stream_test_errors('TERMINATING', [
    'stream' => [
        'error_mode' => StreamErrorMode::Silent,
        'error_store' => StreamErrorStore::Terminating,
    ]
]);

stream_test_errors('AUTO EXCEPTION', [
    'stream' => [
        'error_mode' => StreamErrorMode::Exception,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

stream_test_errors('AUTO ERROR', [
    'stream' => [
        'error_mode' => StreamErrorMode::Error,
        'error_store' => StreamErrorStore::Auto,
    ]
]);

?>
--EXPECTF--
ALL
Error details:
- Message: TestStream::stream_cast is not implemented!
- Code: 70
- Wrapper: user-space
- Terminating: yes
- Count: 2
  [0] 70: TestStream::stream_cast is not implemented!
  [1] 101: Cannot represent a stream of type user-space as a select()able descriptor

NON TERMINATING
Error details:
- Message: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost
- Code: 161
- Wrapper: user-space
- Terminating: no
- Count: 1
  [0] 161: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost

TERMINATING
Error details:
- Message: TestStream::stream_cast is not implemented!
- Code: 70
- Wrapper: user-space
- Terminating: yes
- Count: 2
  [0] 70: TestStream::stream_cast is not implemented!
  [1] 101: Cannot represent a stream of type user-space as a select()able descriptor

AUTO EXCEPTION
EXCEPTION: TestStream::stream_cast is not implemented!
Error details:
- Message: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost
- Code: 161
- Wrapper: user-space
- Terminating: no
- Count: 1
  [0] 161: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost

AUTO ERROR

Warning: fread(): TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost in %s on line %d

Warning: stream_select(): TestStream::stream_cast is not implemented! in %s on line %d

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s on line %d
Error details:
- Message: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost
- Code: 161
- Wrapper: user-space
- Terminating: no
- Count: 1
  [0] 161: TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost
