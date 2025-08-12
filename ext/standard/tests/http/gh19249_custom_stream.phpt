--TEST--
GH-19249 (http context - allow content to be a stream/resource) - custom stream
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php
class MyStream {
    public $context;
    private $counter = 0;

    public function stream_open(string $path, string $mode, int $options, ?string &$opened_path): bool {
        return true;
    }

    public function stream_read(int $count): string|false {
        $this->counter++;
        if ($this->stream_eof()) {
            return false;
        }
        return "test";
    }

    public function stream_eof(): bool {
        return $this->counter == 2;
    }
}

stream_wrapper_register("custom", MyStream::class);

$serverCode = <<<'CODE'
var_dump(getallheaders()['Content-Length']);
echo file_get_contents('php://input');
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

$postData = fopen("custom://", "r");

echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/", false, stream_context_create([
    'http' => [
        'method' => 'POST',
        'header' => [
        	'Content-type: application/x-www-form-urlencoded',
        ],
        'content' => $postData,
    ]
]));
?>
--EXPECTF--
Warning: file_get_contents(): Stream does not support seeking in %s on line %d

Warning: file_get_contents(): MyStream::stream_stat is not implemented! in %s on line %d
string(1) "4"
test
