--TEST--
IntlRelativeDateTimeFormatter cycle management with a custom NumberFormatter
--EXTENSIONS--
intl
--FILE--
<?php

class TestNumberFormatter extends NumberFormatter
{
    public ?IntlRelativeDateTimeFormatter $formatter = null;
}

function collectFormatterCycle(): void
{
    $numberFormatter = new TestNumberFormatter('en', NumberFormatter::DECIMAL);
    $formatter = new IntlRelativeDateTimeFormatter('en', numberFormatter: $numberFormatter);
    $numberFormatter->formatter = $formatter;

    unset($numberFormatter, $formatter);
    var_dump(gc_collect_cycles());
}

collectFormatterCycle();
collectFormatterCycle();

?>
--EXPECT--
int(2)
int(2)
