--TEST--
GH-11274 (POST/PATCH request via file_get_contents + stream_context_create switches to GET after a HTTP 308 redirect)
--INI--
allow_url_fopen=1
--CONFLICTS--
server
--FILE--
<?php
$serverCode = <<<'CODE'
$uri = $_SERVER['REQUEST_URI'];
if (isset($_GET["desired_status"]) && $uri[strlen($uri) - 1] !== '/') {
    $desired_status = (int) $_GET["desired_status"];
    http_response_code($desired_status);
    header("Location: $uri/");
    exit;
}

echo "method: ", $_SERVER['REQUEST_METHOD'], "; body: ", file_get_contents('php://input'), "\n";
CODE;

include __DIR__."/../../../../sapi/cli/tests/php_cli_server.inc";
php_cli_server_start($serverCode, null, []);

foreach ([null, 301, 302, 307, 308] as $status) {
    if (is_null($status)) {
        echo "-- Testing unredirected request --\n";
    } else {
        echo "-- Testing redirect status code $status --\n";
    }
    $suffix = $status ? "?desired_status=$status" : "";
    echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/test$suffix", false, stream_context_create(['http' => ['method' => 'POST', 'header' => 'Content-type: application/x-www-form-urlencoded', 'content' => http_build_query(['hello' => 'world'])]]));
    echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/test$suffix", false, stream_context_create(['http' => ['method' => 'PATCH', 'header' => 'Content-type: application/x-www-form-urlencoded', 'content' => http_build_query(['hello' => 'world'])]]));
    echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/test/$suffix", false, stream_context_create(['http' => ['method' => 'POST', 'header' => 'Content-type: application/x-www-form-urlencoded', 'content' => http_build_query(['hello' => 'world'])]]));
    echo file_get_contents("http://" . PHP_CLI_SERVER_ADDRESS . "/test/$suffix", false, stream_context_create(['http' => ['method' => 'PATCH', 'header' => 'Content-type: application/x-www-form-urlencoded', 'content' => http_build_query(['hello' => 'world'])]]));
}
?>
--EXPECT--
-- Testing unredirected request --
method: POST; body: hello=world
method: PATCH; body: hello=world
method: POST; body: hello=world
method: PATCH; body: hello=world
-- Testing redirect status code 301 --
method: GET; body: 
method: GET; body: 
method: GET; body: 
method: GET; body: 
-- Testing redirect status code 302 --
method: GET; body: 
method: GET; body: 
method: GET; body: 
method: GET; body: 
-- Testing redirect status code 307 --
method: POST; body: hello=world
method: PATCH; body: hello=world
method: POST; body: hello=world
method: PATCH; body: hello=world
-- Testing redirect status code 308 --
method: POST; body: hello=world
method: PATCH; body: hello=world
method: POST; body: hello=world
method: PATCH; body: hello=world
