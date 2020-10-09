--TEST--
Bug #63911 (Ignore conflicting trait methods originationg from identical sub traits)
--FILE--
<?php
trait A
{
    public function a(){
        echo 'Done';
    }
}
trait B
{
    use A;
}
trait C
{
    use A;
}
class D
{
    use B, C;
}

(new D)->a();
?>
--EXPECT--
Done
