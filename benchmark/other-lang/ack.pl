use integer;

sub Ack {
    return $_[0] ? ($_[1] ? Ack($_[0]-1, Ack($_[0], $_[1]-1))
		    : Ack($_[0]-1, 1))
	: $_[1]+1;
}

my $NUM = 9;
$NUM = 1 if ($NUM < 1);
my $ack = Ack(3, $NUM);
