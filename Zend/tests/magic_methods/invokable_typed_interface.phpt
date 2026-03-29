--TEST--
Interface extending Invokable can fix __invoke() signature
--FILE--
<?php

/* Interface extending Invokable with a fixed signature */
interface RequestHandler extends Invokable {
    public function __invoke(string $request): string;
}

class MyHandler implements RequestHandler {
    public function __invoke(string $request): string {
        return "handled: " . $request;
    }
}

$h = new MyHandler();
var_dump($h instanceof Invokable);
var_dump($h instanceof RequestHandler);
var_dump($h("test"));

/* Multiple levels of interface inheritance */
interface JsonHandler extends RequestHandler {}

class MyJsonHandler implements JsonHandler {
    public function __invoke(string $request): string {
        return json_encode(["handled" => $request]);
    }
}

$jh = new MyJsonHandler();
var_dump($jh instanceof Invokable);
var_dump($jh instanceof RequestHandler);
var_dump($jh instanceof JsonHandler);
var_dump($jh('{"key":"value"}'));

/* Can be used as parameter type with the fixed signature interface */
function dispatch(RequestHandler $handler, string $req): string {
    return $handler($req);
}

echo dispatch(new MyHandler(), "hello") . "\n";

?>
--EXPECT--
bool(true)
bool(true)
string(13) "handled: test"
bool(true)
bool(true)
bool(true)
string(33) "{"handled":"{\"key\":\"value\"}"}"
handled: hello
