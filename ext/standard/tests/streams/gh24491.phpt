--TEST--
GH-24491 (Stream error_handler given as 'Class::method' turns the real exception into a TypeError)
--FILE--
<?php
class H {
    public static function handle(array $errors): void {
        echo "handler called\n";
    }
}

function run(array $options): void {
    try {
        fopen('/nonexistent/dir/file', 'r', false, stream_context_create(['stream' => $options]));
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
        var_dump($e->getPrevious());
    }
}

run(['error_mode' => StreamErrorMode::Exception, 'error_handler' => 'H::handle']);

set_error_handler(function (int $errno, string $errstr) {
    throw new ErrorException($errstr);
});
run(['error_handler' => ['H', 'handle']]);
?>
--EXPECT--
StreamException: Failed to open stream: No such file or directory
NULL
ErrorException: fopen(): Failed to open stream: No such file or directory
NULL
