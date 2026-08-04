--TEST--
Bug #67359 (Segfault in recursiveDirectoryIterator)
--FILE--
<?php
try
{
    $rdi = new RecursiveDirectoryIterator(__DIR__,  FilesystemIterator::SKIP_DOTS | FilesystemIterator::UNIX_PATHS);
    $it = new RecursiveIteratorIterator( $rdi );
    $it->seek(1);
    while( $it->valid())
    {
        if( $it->isFile() )
        {
            $it->current();
        }

        $it->next();
    }

    $it->current();
}
catch(Exception $e)
{
}
echo "okey"
?>
--EXPECT--
okey
