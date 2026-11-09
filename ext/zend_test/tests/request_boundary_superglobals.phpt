--TEST--
Request boundary releases the objects stored in superglobals when the next request begins, except $_ENV
--EXTENSIONS--
zend_test
--FILE--
<?php
final class Secret
{
    /** @var list<string> */
    public static array $log = [];

    public function __construct(public string $value)
    {
    }

    public function __destruct()
    {
        self::$log[] = "destroyed {$this->value}";
    }
}

final class Mover
{
    public function __destruct()
    {
        $_GET['moved'] = new Secret('moved');
    }
}

$_GET['secret'] = new Secret('get');
$_POST['secret'] = new Secret('post');
$_COOKIE['secret'] = new Secret('cookie');
$_SERVER['secret'] = new Secret('server');
$_REQUEST['secret'] = new Secret('request');
$_FILES['mover'] = new Mover();
$_ENV['secret'] = new Secret('env');

zend_test_request_boundary(static function (): void {
    Secret::$log[] = "between, still readable: {$_GET['secret']->value}";
    $_POST['between'] = new Secret('between');
});

echo implode("\n", Secret::$log), "\n";
var_dump(isset($_GET['moved']), isset($_POST['between']), $_ENV['secret']->value);
?>
--EXPECT--
between, still readable: get
destroyed get
destroyed post
destroyed between
destroyed cookie
destroyed server
destroyed request
destroyed moved
bool(false)
bool(false)
string(3) "env"
