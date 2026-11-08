--TEST--
GH-11821 (exit() while autoloading a classmap class)
--EXTENSIONS--
soap
--INI--
soap.wsdl_cache_enabled=0
--FILE--
<?php
register_shutdown_function(function () {
    echo "shutdown\n";
});
spl_autoload_register(function ($class) {
    echo "autoload $class\n";
    exit;
});

function f() {
    return ['x' => 5, 'y' => 6];
}

class LocalSoapClient extends SoapClient {
    private $server;

    function __construct($wsdl, $options) {
        parent::__construct($wsdl, $options);
        $this->server = new SoapServer($wsdl);
        $this->server->addFunction("f");
    }

    function __doRequest($request, $location, $action, $version, $one_way = false, ?string $uriParserClass = null): string {
        ob_start();
        $this->server->handle($request);
        return ob_get_clean();
    }
}

$client = new LocalSoapClient(__DIR__ . "/classmap003.wsdl", ['classmap' => ['A' => 'MissingA']]);
$client->f();
echo "not reached\n";
?>
--EXPECT--
autoload MissingA
shutdown
