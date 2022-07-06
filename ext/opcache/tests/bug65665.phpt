--TEST--
Bug #65665 (Exception not properly caught when opcache enabled)
--INI--
opcache.enable=1
opcache.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
function foo() {
    try
    {
        switch (1)
        {
        case 0:
            try
            {

            }
            catch (Exception $e)
            {

            }

            break;

        case 1:
            try
            {
                throw new Exception('aaa');
            }
            catch (Exception $e)
            {
                echo "correct\n";
            }

            break;
        }
    }
    catch (Exception $e)
    {
        echo "wrong\n";
    }
    return;
}

function foo1() {
    try
    {
        switch (1)
        {
        case 0:
            try
            {

            }
            catch (Exception $e)
            {
dummy:
                echo "ect\n";
            }

            break;

        case 1:
            try
            {
                throw new Exception('aaa');
            }
            catch (Exception $e)
            {
                echo "corr";
                goto dummy;
            }
            break;
        }
    }
    catch (Exception $e)
    {
        echo "wrong\n";
    }
    return;
}

function foo2() {
    try
    {
        switch (1)
        {
        case 0:
            try
            {
dummy:
                throw new Exception('aaa');
            }
            catch (Exception $e)
            {
                echo "correct\n";
            }

            break;

        case 1:
            goto dummy;
            break;
        }
    }
    catch (Exception $e)
    {
        echo "wrong\n";
    }
    return;
}
foo();foo1();foo2();
?>
--EXPECT--
correct
correct
correct
