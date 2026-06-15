--TEST--
GH-20469: Child delayed variance can resolve parent before direct delayed resolution
--DESCRIPTION--
This variant ensures the cacheability check after load_delayed_classes() is
needed. Loading the delayed child resolves the parent class's variance
obligations reentrantly, so the parent no longer has ZEND_ACC_UNRESOLVED_VARIANCE
when control returns from load_delayed_classes(). The parent was still used while
nearly linked, and must not be inserted into the inheritance cache.
--EXTENSIONS--
opcache
--CONFLICTS--
server
--FILE--
<?php
$dir = __DIR__ . '/gh20469_child_variance_resolves_parent';
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
 * ParentBeingLinked -> CovariantReturnWithTrait -> RequiresRootReturnTrait
 * -> ChildOfParentBeingLinked -> ParentBeingLinked.
 *
 * ChildOfParentBeingLinked also has delayed variance, so resolving the child's
 * dependency on ParentBeingLinked can resolve ParentBeingLinked before it
 * reaches its direct resolve_delayed_variance_obligations() call.
 */
file_put_contents($dir . '/test1.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
// Prime the inheritance cache with the full dependency cycle.
echo \APP\ChildOfParentBeingLinked::SOME_CONSTANT;
PHP);

file_put_contents($dir . '/test2.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
// Link ParentBeingLinked first. During load_delayed_classes(), loading the
// delayed child resolves ParentBeingLinked's variance obligations reentrantly.
echo \APP\ParentBeingLinked::SOME_CONSTANT;
$i = new \APP\ChildOfParentBeingLinked();
var_dump($i->test());
PHP);

file_put_contents($dir . '/test3.php', <<<'PHP'
<?php
require __DIR__ . '/autoload.php';
// Replay the cache state created by test2. If ParentBeingLinked was cached even
// though it was used while nearly linked, this request fails before test() runs.
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

    // CovariantReturnWithTrait is unavailable when this method is checked,
    // putting ParentBeingLinked into delayed variance resolution.
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
    // MoreSpecificReturn is also unavailable when the child is linked. Resolving
    // this child's delayed variance obligation recursively resolves the parent.
    function createResult(): MoreSpecificReturn
    {
    }
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

    // This pulls ChildOfParentBeingLinked into the delayed autoload queue while
    // ParentBeingLinked is nearly linked.
    function build(): ChildOfParentBeingLinked
    {
    }
}
PHP);

file_put_contents($dir . '/classes/MoreSpecificReturn.php', <<<'PHP'
<?php
namespace APP;

class MoreSpecificReturn extends CovariantReturnWithTrait
{
}
PHP);

include 'php_cli_server.inc';
$ini = trim((string) getenv('TEST_PHP_EXTRA_ARGS'));
$ini .= ($ini !== '' ? ' ' : '') . '-d opcache.enable=1 -d opcache.enable_cli=1 -d opcache.file_update_protection=0';
php_cli_server_start($ini);

echo rtrim(file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh20469_child_variance_resolves_parent/test1.php'), "\n"), "\n";
echo rtrim(file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh20469_child_variance_resolves_parent/test2.php'), "\n"), "\n";
echo rtrim(file_get_contents('http://' . PHP_CLI_SERVER_ADDRESS . '/gh20469_child_variance_resolves_parent/test3.php'), "\n"), "\n";
?>
--CLEAN--
<?php
$dir = __DIR__ . '/gh20469_child_variance_resolves_parent';
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
3NULL
