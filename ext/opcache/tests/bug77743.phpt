--TEST--
Bug #77743: Incorrect pi node insertion for jmpznz with identical successors
--FILE--
<?php
class Toto
{
    public function process1()
    {
        $keep_products = [1, 2, 3, 4];
        foreach ($keep_products as $k => $v)
        {
            $id_country = myRet(45);
            if ($id_country === false && false)
            {
            }

            var_dump($id_country === false);
        }
    }
}

function myRet($x){
    return $x;
}

$toto = new Toto();
$toto->process1();

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
