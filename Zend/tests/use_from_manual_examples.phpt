--TEST--
use ... from ... mirrors manual aliasing/importing examples
--FILE--
<?php

namespace My\Full;

class Classname {}
class NSname {}

namespace My\Full\NSname\subns;

function func(): void
{
    echo "NSname subns func\n";
}

namespace some\namespace;

class ClassA {}
class ClassB {}
class ClassC {}

function fn_a(): void
{
    echo "fn_a\n";
}

function fn_b(): void
{
    echo "fn_b\n";
}

function fn_c(): void
{
    echo "fn_c\n";
}

const ConstA = "ConstA";
const ConstB = "ConstB";
const ConstC = "ConstC";

namespace Mizo\Web\Php;

class WebSite {}

namespace Mizo\Web\JS;

function printTotal(): void
{
    echo "printTotal\n";
}

const BUAIKUM = "BUAIKUM";

namespace foo;

use Classname from My\Full as Another, NSname from My\Full;
use {ClassA, ClassB, ClassC as C} from some\namespace;
use function {fn_a, fn_b, fn_c} from some\namespace;
use const {ConstA, ConstB, ConstC} from some\namespace;
use {Php\WebSite, function JS\printTotal, const JS\BUAIKUM} from Mizo\Web;

new Another();
echo "Another OK\n";

NSname\subns\func();
echo "NSname alias OK\n";

new ClassA();
new ClassB();
new C();
echo "Group classes OK\n";

fn_a();
fn_b();
fn_c();
echo "Group functions OK\n";

echo ConstA, " ", ConstB, " ", ConstC, "\n";
echo "Group consts OK\n";

new WebSite();
printTotal();
echo BUAIKUM, "\n";
echo "Mixed group OK\n";

?>
--EXPECT--
Another OK
NSname subns func
NSname alias OK
Group classes OK
fn_a
fn_b
fn_c
Group functions OK
ConstA ConstB ConstC
Group consts OK
printTotal
BUAIKUM
Mixed group OK
