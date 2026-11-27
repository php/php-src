--TEST--
GH-24534: Inheritance cache must not record an unlinked child as a dependency
--CREDITS--
Wim Winterberg
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/gh24534';
@mkdir($dir . '/src', 0777, true);

file_put_contents($dir . '/autoload.php', <<<'PHP'
<?php
spl_autoload_register(static function (string $class): void {
    $file = __DIR__ . '/src/' . $class . '.php';
    if (is_file($file)) {
        require $file;
    }
});
PHP);

file_put_contents($dir . '/src/EntityInterface.php', <<<'PHP'
<?php
interface EntityInterface
{
    public static function getById(int $id): ?self;
}
PHP);

file_put_contents($dir . '/src/Entity.php', <<<'PHP'
<?php
abstract class Entity implements EntityInterface
{
    protected array $data = [];
}
PHP);

file_put_contents($dir . '/src/UserBase.php', <<<'PHP'
<?php
abstract class UserBase extends Entity
{
    public const TABLE = 'user';

    protected ?int $userId = null;

    public static function getById(int $id): ?User
    {
        return null;
    }
}
PHP);

file_put_contents($dir . '/src/User.php', <<<'PHP'
<?php
class User extends UserBase
{
}
PHP);

file_put_contents($dir . '/base-first.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
echo 'base-first: ', UserBase::TABLE, PHP_EOL;
PHP);

file_put_contents($dir . '/child-first.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
echo 'child-first: ', User::TABLE, PHP_EOL;
PHP);

include 'php_cli_server.inc';
$ini = trim((string) getenv('TEST_PHP_EXTRA_ARGS'));
$ini .= ($ini !== '' ? ' ' : '') . '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.file_update_protection=0';
php_cli_server_start($ini);

foreach (['base-first', 'child-first', 'base-first'] as $page) {
    echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh24534/' . $page . '.php');
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh24534';
foreach (['src/EntityInterface.php', 'src/Entity.php', 'src/UserBase.php', 'src/User.php', 'autoload.php', 'base-first.php', 'child-first.php'] as $file) {
    @unlink($dir . '/' . $file);
}
@rmdir($dir . '/src');
@rmdir($dir);
?>
--EXPECT--
base-first: user
child-first: user
base-first: user
