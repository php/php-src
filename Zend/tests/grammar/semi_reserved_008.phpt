--TEST--
Testing with comments around semi-reserved names (not intended to be legible)
--FILE--
<?php

trait TraitA
{
    public static function list(){ echo __METHOD__, PHP_EOL; }
    public static function /* comment */ catch(){ echo __METHOD__, PHP_EOL; }
    private static function // comment
        throw(){ echo __METHOD__, PHP_EOL; }
    private static function
    # comment
    self(){ echo __METHOD__, PHP_EOL; }
}

trait TraitB
{
    public static function exit(){ echo __METHOD__, PHP_EOL; }
    protected static function try(){ echo __METHOD__, PHP_EOL; }
}

class Foo
{
    use TraitA {
        TraitA::
            //
            /** doc comment */
            #
        catch /* comment */
            // comment
            # comment
        insteadof TraitB;

        TraitA::list as public /**/ foreach;
    }

    use TraitB {
        try /*comment*/ as public attempt;
        exit // comment
            as/*comment*/die; // non qualified
        \TraitB::exit as bye; // full qualified
        namespace\TraitB::exit #
        as byebye; // even more full qualified
        TraitB
            ::
            /**  */
            exit as farewell; // full qualified with weird spacing
    }
}

Foo /**/
#
//
/**  */
::
/**/
#
//
/**  */
attempt();

echo PHP_EOL, "Done", PHP_EOL;
?>
--EXPECT--
TraitB::try

Done
