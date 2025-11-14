--TEST--
#[\Override]: Constants - no parent interface
--FILE--
<?php

interface IFace {
    #[\Override]
    public const I = 'I';
}

echo "Done";

?>
--EXPECTF--
Fatal error: IFace::I has #[\Override] attribute, but no matching parent constant exists in %s on line %d
