--TEST--
GH-20469: Inheritance cache with reentrant autoloading must preserve inherited methods
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/gh20469_inherited_method';
@mkdir($dir . '/classes', 0777, true);

file_put_contents($dir . '/autoload.php', <<<'PHP'
<?php
spl_autoload_register(function ($class) {
    $prefix = 'APP\\';
    if (strncmp($class, $prefix, strlen($prefix)) === 0) {
        require __DIR__ . '/classes/' . substr($class, strlen($prefix)) . '.php';
    }
});
PHP);

/* The dependency cycle is:
 * ChildOfParentBeingLinked -> ParentBeingLinked -> CovariantReturnWithTrait
 * -> RequiresRootReturnTrait -> ChildOfParentBeingLinked.
 */
file_put_contents($dir . '/test1.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
echo \APP\ChildOfParentBeingLinked::SOME_CONSTANT;
PHP);

file_put_contents($dir . '/test2.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
echo \APP\ParentBeingLinked::SOME_CONSTANT;
$i = new \APP\ChildOfParentBeingLinked();
var_dump($i->test());
PHP);

file_put_contents($dir . '/classes/RootForTraitReturn.php', <<<'PHP'
<?php
namespace APP;

class RootForTraitReturn
{
    function createResult(): BaseCovariantReturn
    {
    }

    function test() {}
}
PHP);

file_put_contents($dir . '/classes/ParentBeingLinked.php', <<<'PHP'
<?php
namespace APP;

class ParentBeingLinked extends RootForTraitReturn
{
    public const SOME_CONSTANT = 3;

    function createResult(): CovariantReturnWithTrait
    {
    }
}
PHP);

file_put_contents($dir . '/classes/ChildOfParentBeingLinked.php', <<<'PHP'
<?php
namespace APP;

class ChildOfParentBeingLinked extends ParentBeingLinked
{
}
PHP);

file_put_contents($dir . '/classes/BaseCovariantReturn.php', <<<'PHP'
<?php
namespace APP;

abstract class BaseCovariantReturn
{
}
PHP);

file_put_contents($dir . '/classes/RequiresRootReturnTrait.php', <<<'PHP'
<?php
namespace APP;

trait RequiresRootReturnTrait
{
    abstract function build(): RootForTraitReturn;
}
PHP);

file_put_contents($dir . '/classes/CovariantReturnWithTrait.php', <<<'PHP'
<?php
namespace APP;

class CovariantReturnWithTrait extends BaseCovariantReturn
{
    use RequiresRootReturnTrait;

    function build(): ChildOfParentBeingLinked
    {
    }
}
PHP);

include 'php_cli_server.inc';
$ini = trim((string) getenv('TEST_PHP_EXTRA_ARGS'));
$ini .= ($ini !== '' ? ' ' : '') . '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.file_update_protection=0';
php_cli_server_start($ini);

echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh20469_inherited_method/test1.php'), "\n";
echo file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh20469_inherited_method/test2.php'), "\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh20469_inherited_method';
if (is_dir($dir)) {
    $iterator = new RecursiveIteratorIterator(
        new RecursiveDirectoryIterator($dir, RecursiveDirectoryIterator::SKIP_DOTS),
        RecursiveIteratorIterator::CHILD_FIRST
    );
    foreach ($iterator as $file) {
        if ($file->isDir()) {
            rmdir($file->getPathname());
        } else {
            unlink($file->getPathname());
        }
    }
    rmdir($dir);
}
?>
--EXPECT--
3
3NULL
