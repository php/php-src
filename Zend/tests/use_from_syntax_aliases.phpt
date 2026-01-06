--TEST--
use ... as/alias ordering tests
--FILE--
<?php

namespace My\Full {
    class Classname {
        public static function whoami() { return "My\\Full\\Classname"; }
    }
}

namespace {
    // 'from' before 'as'
    use Classname from My\Full as Other;
    echo Other::whoami() . "\n";

    // 'as' before 'from'
    use Classname as Other2 from My\Full;
    echo Other2::whoami() . "\n";

    // Fully-qualified namespace with alias after
    use Classname from \My\Full as Fully;
    echo Fully::whoami() . "\n";

    // Fully-qualified namespace with 'as' before 'from'
    use Classname as Fully2 from \My\Full;
    echo Fully2::whoami() . "\n";
}

?>
--EXPECT--
My\Full\Classname
My\Full\Classname
My\Full\Classname
My\Full\Classname
