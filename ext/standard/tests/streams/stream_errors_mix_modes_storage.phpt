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

function stream_test_errors($title, $context) {
    $stream = fopen('test://foo', 'r', false, $context);
    try {
        echo $title . "\n";
        $readin = fopen( 'php://stdin', 'r' );
        $data   = fread( $stream, 10 );

        $read   = [$readin, $stream];
        $write  = NULL;
        $except = NULL;
        stream_select($read, $write, $except, 0);
    } catch (StreamException $e) {
        echo 'EXCEPTION: ' . $e->getMessage() . "\n";
    }

    $errors = stream_get_errors($stream);
    var_dump($errors);
}

stream_test_errors('ALL', stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_store' => STREAM_ERROR_STORE_ALL,
    ]
]));
stream_test_errors('NON TERMINAL', stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_store' => STREAM_ERROR_STORE_NON_TERMINAL,
    ]
]));
stream_test_errors('TERMINAL', stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_SILENT,
        'error_store' => STREAM_ERROR_STORE_TERMINAL,
    ]
]));
stream_test_errors('AUTO EXCEPTION', stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_EXCEPTION,
        'error_store' => STREAM_ERROR_STORE_AUTO,
    ]
]));
stream_test_errors('AUTO ERROR', stream_context_create([
    'stream' => [
        'error_mode' => STREAM_ERROR_MODE_ERROR,
        'error_store' => STREAM_ERROR_STORE_AUTO,
    ]
]));

?>
--EXPECTF--
ALL
array(3) {
  [0]=>
  array(5) {
    ["message"]=>
    string(113) "TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost"
    ["code"]=>
    int(161)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(false)
    ["wrapper"]=>
    string(10) "user-space"
  }
  [1]=>
  array(5) {
    ["message"]=>
    string(43) "TestStream::stream_cast is not implemented!"
    ["code"]=>
    int(70)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(true)
    ["wrapper"]=>
    string(10) "user-space"
  }
  [2]=>
  array(5) {
    ["message"]=>
    string(73) "Cannot represent a stream of type user-space as a select()able descriptor"
    ["code"]=>
    int(101)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(true)
    ["wrapper"]=>
    string(10) "user-space"
  }
}
NON TERMINAL
array(1) {
  [0]=>
  array(5) {
    ["message"]=>
    string(113) "TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost"
    ["code"]=>
    int(161)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(false)
    ["wrapper"]=>
    string(10) "user-space"
  }
}
TERMINAL
array(2) {
  [0]=>
  array(5) {
    ["message"]=>
    string(43) "TestStream::stream_cast is not implemented!"
    ["code"]=>
    int(70)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(true)
    ["wrapper"]=>
    string(10) "user-space"
  }
  [1]=>
  array(5) {
    ["message"]=>
    string(73) "Cannot represent a stream of type user-space as a select()able descriptor"
    ["code"]=>
    int(101)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(true)
    ["wrapper"]=>
    string(10) "user-space"
  }
}
AUTO EXCEPTION
EXCEPTION: Cannot represent a stream of type user-space as a select()able descriptor
array(1) {
  [0]=>
  array(5) {
    ["message"]=>
    string(113) "TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost"
    ["code"]=>
    int(161)
    ["severity"]=>
    int(2)
    ["terminal"]=>
    bool(false)
    ["wrapper"]=>
    string(10) "user-space"
  }
}
AUTO ERROR

Warning: fread(): TestStream::stream_read - read 10 bytes more data than requested (8202 read, 8192 max) - excess data will be lost in %s on line %d

Warning: stream_select(): TestStream::stream_cast is not implemented! in %s on line %d

Warning: stream_select(): Cannot represent a stream of type user-space as a select()able descriptor in %s on line %d
array(0) {
}
