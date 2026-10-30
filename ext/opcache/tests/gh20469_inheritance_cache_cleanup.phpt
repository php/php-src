--TEST--
GH-20469: Skipped inheritance cache cleanup must ignore non-cacheable classes
--DESCRIPTION--
Autoloading the parent makes the child use the runtime class-linking path, but
the child does not enter inheritance-cache construction. Under ASAN, the
uninitialized inheritance_cache field is filled with non-zero bytes. Skipped
cache insertion must not treat that value as a temporary dependency table.
--EXTENSIONS--
opcache
--FILE--
<?php
spl_autoload_register(function ($class) {
    if ($class === 'ParentForSkippedInheritanceCacheCleanup') {
        eval('class ParentForSkippedInheritanceCacheCleanup {}');
    }
});

eval('class ChildForSkippedInheritanceCacheCleanup extends ParentForSkippedInheritanceCacheCleanup {}');
echo "ok\n";
?>
--EXPECT--
ok
