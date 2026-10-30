--TEST--
Lazy objects: RFC example 012
--FILE--
<?php

// User code

class ClientFactory
{
    public function __construct(
        private string $hostname,
        private string $credentials,
    ) {}

    public function createClient() {
        return new Client($this->hostname, $this->credentials);
    }
}

class Client
{
    public function __construct(
        private string $hostname,
        private string $credentials,
    ) {}

    public function doSomething()
    {
        printf("doSomething() (hostname: %s)\n", $this->hostname);
    }
}

// Symfony code

class Container
{
    public function getClientFactoryService(): ClientFactory
    {
        return new ClientFactory('127.0.0.1', 'secret');
    }

    public function getClientService(): Client
    {
        $reflector = new ReflectionClass(Client::class);

        $client = $reflector->newLazyProxy(function () {
            $clientFactory = $this->getClientFactoryService();
            return $clientFactory->createClient();
        });

        return $client;
    }
}

$container = new Container();
$service = $container->getClientService();
var_dump($service);
$service->doSomething();
var_dump($service);

?>
==DONE==
--EXPECTF--
lazy proxy object(Client)#%d (0) {
  ["hostname":"Client":private]=>
  uninitialized(string)
  ["credentials":"Client":private]=>
  uninitialized(string)
}
doSomething() (hostname: 127.0.0.1)
lazy proxy object(Client)#%d (1) {
  ["instance"]=>
  object(Client)#%d (2) {
    ["hostname":"Client":private]=>
    string(9) "127.0.0.1"
    ["credentials":"Client":private]=>
    string(6) "secret"
  }
}
==DONE==
